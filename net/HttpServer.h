//
// Created by Fire on 2025/9/21.
//

#ifndef SNOW_HTTP_SERVER_HTTPSERVER_H
#define SNOW_HTTP_SERVER_HTTPSERVER_H

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <chrono>
#include <functional>
#include <map>
#include <random>
#include <string>
#include <thread>
#include <utility>

#include "http/http_message.h"
#include "http/uri.h"
#include "ThreadPool.h"


namespace snow {

    constexpr size_t
    kMaxBufferSize = 4096;

    struct EvenData {
        EventData() : fd(0), length(0), cursor(0), buffer() {}

        int fd;
        size_t length;
        size_t cursor;
        char buffer[kMaxBufferSize];
    };

    using HttpRequestHandler_t = std::function<

    HttpResponse(const HttpRequest &)

    >;
    using CoroHttpRequestHandler_t = std::function<

    CoroTask(const HttpRequest &)

    >;

    class HttpServer {
    public:
        explicit HttpServer(const std::string &host, std::uint16_t port);

        ~HttpServer() = default;

        HttpServer() = default;

        HttpServer(HttpServer &&) = default;

        HttpServer &operator=(HttpServer &&) = default;

        void Start();

        void Stop();

        void RegisterHttpRequestHandler(const std::string &path, HttpMethod method,
                                        const CoroHttpRequestHandler_t callback) {

        }

    private:
        static constexpr int kBackLogSize = 1000;
        static constexpr int kMaxConnections = 10000;
        static constexpr int kMaxEvents = 10000;

        std::string host_;
        std::uint16_t port_;
        int sock_fd;
        bool running_;
        std::thread listener_thread_;

        ThreadPool thread_pool_;

        std::map <Uri, std::map<HttpMethod, HttpRequestHandler_t>> request_handlers_;
        std::map <Uri, std::map<HttpMethod, CoroHttpRequestHandler_t>> coro_request_handlers_;

        std::mt19937 rng_;
        std::uniform_int_distribution<int> sleep_times_;

        void CreateSocket();

        void SetUpEpoll();

        void Listen();

        void ProcessEvents();

        void HandleEpollEvent(int epoll_fd, EventData *event, std::uint32_t events);

        void HandleHttpData(const EventData &request, EventData *response);

        HttpResponse HandleHttpRequest(const HttpRequest &request);

        CoroTask HandleCoroHttpRequest(const HttpRequest &request);

        void control_epoll_envent(int epoll_fd, int op, int fd, std::uint32_t events = 0, void *data = nullptr);
    };

} // snow

#endif //SNOW_HTTP_SERVER_HTTPSERVER_H
