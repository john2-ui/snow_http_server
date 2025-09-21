//
// Created by Fire on 2025/9/20.
//

#ifndef SNOW_HTTP_SERVER_CORO_HTTP_HANDLER_H
#define SNOW_HTTP_SERVER_CORO_HTTP_HANDLER_H

#include <iostream>
#include <functional>
#include <experimental/coroutine>
#include "http/http_message.h"

namespace snow {
    // CoroTask is the return type for our coroutine. It acts as a handle
    // to manage the coroutine from the caller's perspective. It doesn't
    // contain the coroutine's logic itself, but provides an interface to
    // control its lifecycle (e.g., resume, check status) and get its result.
    class CoroTask {
    public:
        // The promise_type is a special struct the C++ compiler looks for.
        // It defines the behavior of the coroutine at different stages of its life.
        struct promise_type {
            // This is where the coroutine's final result (the HttpResponse) is stored.
            // When the coroutine executes `co_return some_response;`, the value is
            // moved into this member variable.
            HttpResponse response;

            // This function is called by the compiler to create the object
            // that is returned from the coroutine function (i.e., the CoroTask handle).
            // It links our CoroTask object to the coroutine's internal state.
            auto get_return_object() {
                return CoroTask{std::experimental::coroutine_handle<promise_type>::from_promise(*this)};
            }

            // This function is called at the beginning of the coroutine's execution.
            // Returning `suspend_always{}` tells the compiler to suspend the coroutine
            // immediately after it is created, before executing any of its body code.
            // This allows the caller to explicitly start the coroutine with `resume()`.
            auto initial_suspend() { return std::experimental::suspend_always{}; }

            // This function is called just before the coroutine's function body
            // is about to finish. Returning `suspend_always{}` tells the compiler
            // to suspend the coroutine. This is critical as it prevents the
            // coroutine from being automatically destroyed, giving the caller a
            // chance to retrieve the result.
            auto final_suspend() { return std::experimental::suspend_always{}; }

            // This function is called when the coroutine executes a `co_return` statement.
            // The value returned by the coroutine is passed to this function.
            void return_value(HttpResponse value) { response = std::move(value); }

            // This is a mandatory function in the promise_type. It is called if
            // an unhandled exception is thrown inside the coroutine's body.
            // For this simple example, we do nothing with it.
            void unhandled_exception() {}
        };

        // This is the constructor for our CoroTask handle. It takes a coroutine_handle
        // which points to the coroutine's internal state.
        explicit CoroTask(std::experimental::coroutine_handle<promise_type> handle) : handle_(handle) {}

        // The destructor cleans up the coroutine's state by destroying the handle.
        // This is necessary because final_suspend() prevented the automatic cleanup.
        ~CoroTask() { if (handle_) handle_.destroy(); }

        // This function resumes the coroutine from its suspended state.
        void resume() { handle_.resume(); }

        // Checks if the coroutine has finished its execution.
        bool done() { return handle_.done(); }

        // Retrieves the final HttpResponse value from the coroutine's promise.
        HttpResponse get_response() { return handle_.promise().response; }

    private:
        // The coroutine handle. It points to the coroutine's state,
        // which lives on the heap.
        std::experimental::coroutine_handle<promise_type> handle_;
    };

    // This is the coroutine function. It's marked as a coroutine because
    // its return type is `CoroTask`. When the compiler sees this, it
    // will transform the function's body to support coroutine behavior
    // (e.g., initial suspend, final suspend, co_return, co_await).
    CoroTask handle_coroutine_request(const HttpRequest& request);

} // snow

#endif //SNOW_HTTP_SERVER_CORO_HTTP_HANDLER_H