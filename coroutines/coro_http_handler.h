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
    class CoroTask {
    public:
        struct promise_type {
            HttpResponse response;

            auto get_return_object() {
                return CoroTask{std::experimental::coroutine_handle<promise_type>::from_promise(*this)};
            }

            auto initial_suspend() { return std::experimental::suspend_always{}; }

            auto final_suspend() { return std::experimental::suspend_always{}; }

            void return_value(HttpResponse value) { response = std::move(value); }

            void unhandled_exception() {}
        };
    };
} //snow

#endif //SNOW_HTTP_SERVER_CORO_HTTP_HANDLER_H
