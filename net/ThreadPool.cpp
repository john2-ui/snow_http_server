//
// Created by Fire on 2025/9/20.
//

#include "ThreadPool.h"

namespace snow {

// The ThreadPool class manages a set of worker threads to execute tasks.
    // Constructor: Initializes the thread pool with a specified number of threads.
    ThreadPool::ThreadPool(size_t threads) : stop(false) {
        // Create the specified number of worker threads.
        for (size_t i = 0; i < threads; ++i) {
            // Emplace a new thread into the 'workers' vector.
            // The thread executes the provided lambda function.
            workers.emplace_back([this] {
                // Each worker thread enters an infinite loop to continuously process tasks.
                while (true) {
                    std::function<void()> task; // Placeholder for the task to be executed.
                    {
                        // Acquire a unique lock on the queue_mutex.
                        // This protects the shared task queue from concurrent access.
                        std::unique_lock <std::mutex> lock(this->queue_mutex);

                        // The thread waits here until a condition is met.
                        // It atomically releases the lock and goes to sleep.
                        // It wakes up and re-acquires the lock when either:
                        // 1. The 'stop' flag is true (shutdown).
                        // 2. The task queue is not empty.
                        this->condition.wait(lock, [this] {
                            return this->stop || !this->tasks.empty();
                        });

                        // After waking up, check if the pool is shutting down
                        // and there are no more tasks.
                        if (this->stop && this->tasks.empty()) {
                            // If so, the thread safely exits its loop.
                            return;
                        }

                        // Get the next task from the front of the queue.
                        // std::move is used for efficiency to transfer ownership.
                        task = std::move(this->tasks.front());
                        // Remove the task from the queue.
                        this->tasks.pop();

                    } // The lock is automatically released here as 'lock' goes out of scope.

                    // Execute the retrieved task outside the lock.
                    // This allows other threads to access the queue while this thread is busy.
                    task();
                }
            });
        }
    }

    ThreadPool::~ThreadPool() {
        {
            std::unique_lock <std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker: workers) {
            worker.join();
        }
    }

    template<class F, class ...Args>
    auto ThreadPool::enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type> {
        // Determine the return type of the function F with arguments Args...
        using return_type = typename std::result_of<F(Args...)>::type;

        // Create a shared pointer to a packaged_task.
        // The packaged_task is specialized to wrap a function that takes no arguments and returns `return_type`.
        // std::bind is used to create a function object that binds the provided function `f` to its arguments `args...`.
        // std::forward is used for perfect forwarding, preserving the lvalue/rvalue nature of the arguments.
        auto task = std::make_shared < std::packaged_task < return_type() >> (
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        // Get the future associated with the packaged_task.
        // This future will be used by the caller to retrieve the result.
        std::future <return_type> res = task->get_future();
        {
            // Acquire a unique lock to protect the shared task queue.
            std::unique_lock <std::mutex> lock(queue_mutex);

            // Check if the thread pool is in a stopped state.
            if (stop) {
                // If so, throw a runtime error to prevent enqueuing new tasks.
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            // Add the task to the queue.
            // We use a lambda to wrap the packaged_task. This lambda has the required `void()` signature
            // for the `std::function` in the queue.
            // The lambda captures the shared pointer `task` by value, ensuring the packaged_task
            // object remains alive until a worker thread processes it.
            tasks.emplace([task]() {
                (*task)(); // Execute the packaged_task. This will run the function `f` and set the result in the future.
            });
        } // The lock is automatically released here.

        // Notify one of the waiting worker threads that a new task is available.
        condition.notify_one();

        // Return the future to the caller.
        return res;
    }


    //模板显式实例化
    template auto ThreadPool::enqueue<std::function < void()>>
    (
    std::function<void()> f
    ) ->
    std::future<void>;
    template auto ThreadPool::enqueue<std::function
    <

    HttpResponse(const HttpRequest &)

    >, const HttpRequest&>(std::function<HttpResponse(const HttpRequest &)>, const HttpRequest&) ->
    std::future<HttpResponse>;

} // snow