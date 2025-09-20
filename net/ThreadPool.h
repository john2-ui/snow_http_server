//
// Created by Fire on 2025/9/20.
//

#ifndef SNOW_HTTP_SERVER_THREADPOOL_H
#define SNOW_HTTP_SERVER_THREADPOOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

#include "http/http_message.h"

namespace snow {

    class ThreadPool {
    public:
        ThreadPool(size_t threads);

        ~ThreadPool();

        //入队函数，这里使用是为了异步处理
        template<class F, class ...Args>
        auto enqueue(F &&f, Args &&... args) -> std::futrue<typename std::result_of<F(Args...)>::type>;

    private:
        std::vector <std::thread> workers;
        //The task queue
        std::queue <std::function<void()>> tasks;

        std::mutex queue_mutex;
        std::condition_variable condition;
        bool stop;
    };

} // snow

#endif //SNOW_HTTP_SERVER_THREADPOOL_H
