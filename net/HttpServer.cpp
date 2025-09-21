//
// Created by Fire on 2025/9/21.
//

#include "HttpServer.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

#inclue <netinet/in.h>

#include <cstring>
#include <sys/eventfd.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>

namespace snow {
    namespace {
// Helper function to set a socket to non-blocking mode
        bool SetNonBlocking(int fd) {
            int flags = fcntl(fd, F_GETFL, 0);
            if (flags == -1) {
                return false;
            }
            flags |= O_NONBLOCK;
            return fcntl(fd, F_SETFL, flags) != -1;
        }
    } // namespace

    HttpServer::HttpServer(const std::string &host, std::uint16_t port)
            : host_(host),
              port_(port),
              sock_fd_(-1),
              running_(false),
              thread_pool_(kThreadPoolSize), // Initializes the thread pool
              rng_(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
              sleep_times_(1, 5) {}

    void HttpServer::Start() {
        running_ = true;
        CreateSocket();
        Listen();
        // The listener thread now manages accepting connections and dispatching
        // tasks to the thread pool. The worker_threads_ array is no longer needed.
        listener_thread_ = std::thread(&HttpServer::Listen, this);
    }

    void HttpServer::Stop() {
        running_ = false;
        if (sock_fd_ != -1) {
            close(sock_fd_);
        }
        if (listener_thread_.joinable()) {
            listener_thread_.join();
        }
        // The ThreadPool destructor will handle joining its worker threads
    }

    void HttpServer::CreateSocket() {
        sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd_ == -1) {
            // Handle error
            return;
        }

        // Set socket options and bind
        int on = 1;
        setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(host_.c_str());
        serv_addr.sin_port = htons(port_);

        if (bind(sock_fd_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) {
            // Handle error
            close(sock_fd_);
            return;
        }

        if (listen(sock_fd_, kBacklogSize) == -1) {
            // Handle error
            close(sock_fd_);
            return;
        }

        SetNonBlocking(sock_fd_);
    }

    void HttpServer::Listen() {
        int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            // Handle error
            return;
        }

        epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = sock_fd_;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd_, &event) == -1) {
            // Handle error
            close(epoll_fd);
            return;
        }

        epoll_event events[kMaxEvents];
        while (running_) {
            int num_events = epoll_wait(epoll_fd, events, kMaxEvents, -1);
            if (num_events == -1) {
                if (errno == EINTR) continue;
                // Handle error
                break;
            }

            for (int i = 0; i < num_events; ++i) {
                if (events[i].data.fd == sock_fd_) {
                    // New connection
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int client_fd;
                    while ((client_fd = accept(sock_fd_, (struct sockaddr *) &client_addr, &client_addr_len)) > 0) {
                        SetNonBlocking(client_fd);

                        // Allocate an EventData for the new client and add to epoll
                        EventData *event_data = new EventData();
                        event_data->fd = client_fd;
                        epoll_event new_event;
                        new_event.events = EPOLLIN | EPOLLET;
                        new_event.data.ptr = event_data;

                        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &new_event) == -1) {
                            delete event_data;
                            close(client_fd);
                        }
                    }
                    if (client_fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
                        // Handle accept error
                    }
                } else {
                    // Data on existing connection
                    EventData *event_data = static_cast<EventData *>(events[i].data.ptr);
                    thread_pool_.enqueue([this, epoll_fd, event_data, events_i = events[i].events]() {
                        HandleEpollEvent(epoll_fd, event_data, events_i);
                    });
                }
            }
        }
        close(epoll_fd);
    }

    void HttpServer::HandleEpollEvent(int epoll_fd, EventData *event, std::uint32_t events) {
        if (events & EPOLLIN) {
            // Read data
            event->length = read(event->fd, event->buffer, kMaxBufferSize);
            if (event->length > 0) {
                EventData response_data;
                HandleHttpData(*event, &response_data);

                // Set up epoll to monitor for write events
                epoll_event write_event;
                write_event.events = EPOLLOUT | EPOLLET;
                write_event.data.ptr = event;
                epoll_ctl(epoll_fd, EPOLL_CTL_MOD, event->fd, &write_event);
            } else {
                // End of file or error, close connection
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event->fd, nullptr);
                close(event->fd);
                delete event;
            }
        } else if (events & EPOLLOUT) {
            // Write data
            size_t written = write(event->fd, event->buffer + event->cursor, event->length - event->cursor);
            if (written < 0) {
                // Handle error
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event->fd, nullptr);
                close(event->fd);
                delete event;
            } else {
                event->cursor += written;
                if (event->cursor == event->length) {
                    // All data written, close connection for now
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event->fd, nullptr);
                    close(event->fd);
                    delete event;
                }
            }
        }
    }

    void HttpServer::HandleHttpData(const EventData &request, EventData *response) {
        HttpRequest http_request;
        HttpParser parser;
        parser.ParseRequest(request.buffer, request.length, &http_request);

        HttpResponse http_response;
        if (coro_request_handlers_.count(http_request.uri())) {
            if (coro_request_handlers_[http_request.uri()].count(http_request.method())) {
                // Found a coroutine handler
                CoroTask task = coro_request_handlers_[http_request.uri()][http_request.method()](http_request);
                task.resume(); // Start the coroutine
                while (!task.done()) {
                    // In a real scenario, this would involve a more sophisticated
                    // event loop to await the coroutine's completion without
                    // blocking the thread. For this example, we'll loop until it's done.
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    task.resume();
                }
                http_response = task.get_response();
            }
        } else if (request_handlers_.count(http_request.uri())) {
            if (request_handlers_[http_request.uri()].count(http_request.method())) {
                // Found a regular synchronous handler
                http_response = request_handlers_[http_request.uri()][http_request.method()](http_request);
            }
        } else {
            // No handler found
            http_response = HttpResponse(404, "Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
        }

        // Convert HttpResponse to raw data and copy to response buffer
        std::string raw_response = http_response.ToString();
        memcpy(response->buffer, raw_response.c_str(), raw_response.size());
        response->length = raw_response.size();
        response->fd = request.fd;
    }
} // snow