//
// Created by Fire on 2025/9/21.
//

#include "http_server.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/ioctl.h>
#include <sys/sendfile.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>

#include "http/http_parser.h"

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
              sock_fd(-1),
              running_(false),
            // Assuming a ThreadPool size is defined somewhere or passed to the constructor.
            // Based on the old code, a constant kThreadPoolSize was used.
            // We'll assume a default of 5 for this example.
              thread_pool_(5),
              rng_(std::chrono::high_resolution_clock::now().time_since_epoch().count()),
              sleep_times_(1, 5) {}

    void HttpServer::Start() {
        running_ = true;
        CreateSocket();
        Listen();
        listener_thread_ = std::thread(&HttpServer::Listen, this);
    }

    void HttpServer::Stop() {
        running_ = false;
        if (sock_fd != -1) {
            close(sock_fd);
        }
        if (listener_thread_.joinable()) {
            listener_thread_.join();
        }
    }

    void HttpServer::CreateSocket() {
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == -1) {
            // Handle error
            return;
        }

        // Set socket options and bind
        int on = 1;
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(host_.c_str());
        serv_addr.sin_port = htons(port_);

        if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
            // Handle error
            close(sock_fd);
            return;
        }

        if (listen(sock_fd, kBackLogSize) == -1) {
            // Handle error
            close(sock_fd);
            return;
        }

        SetNonBlocking(sock_fd);
    }

    void HttpServer::Listen() {
        int epoll_fd = epoll_create1(0);
        if (epoll_fd == -1) {
            // Handle error
            return;
        }

        epoll_event event;
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = sock_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
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
                if (events[i].data.fd == sock_fd) {
                    // New connection
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    int client_fd;
                    while ((client_fd = accept(sock_fd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0) {
                        SetNonBlocking(client_fd);

                        // Allocate an EventData for the new client and add to epoll
                        // Note: EventData and EvenData are typos in the original header.
                        // Assuming EventData is the correct one.
                        EventData* event_data = new EventData();
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
                    EventData* event_data = static_cast<EventData*>(events[i].data.ptr);
                    thread_pool_.enqueue([this, epoll_fd, event_data, events_i = events[i].events]() {
                        HandleEpollEvent(epoll_fd, event_data, events_i);
                    });
                }
            }
        }
        close(epoll_fd);
    }

    void HttpServer::HandleEpollEvent(int epoll_fd, EventData* event, std::uint32_t events) {
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

    void HttpServer::HandleHttpData(const EventData& request, EventData* response) {
        HttpRequest http_request;
        HttpParser parser;
        parser.ParseRequest(request.buffer, request.length, &http_request);

        HttpResponse http_response;
        if (coro_request_handlers_.count(http_request.uri())) {
            if (coro_request_handlers_[http_request.uri()].count(http_request.method())) {
                // Found a coroutine handler
                CoroTask task = coro_request_handlers_[http_request.uri()][http_request.method()](http_request);
                task.resume();
                while (!task.done()) {
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

    HttpResponse HttpServer::HandleHttpRequest(const HttpRequest &request) {
        // This function is still declared but not used in the new implementation.
        // It is kept for completeness as per the header file.
        // The logic is handled by HandleHttpData now.
        return HttpResponse(404, "Not Found", "Handler not implemented.");
    }

    CoroTask HttpServer::HandleCoroHttpRequest(const HttpRequest &request) {
        // This function is also not used directly but declared.
        // The logic is handled by HandleHttpData now.
        co_return HttpResponse(404, "Not Found", "Coro handler not implemented.");
    }

    void HttpServer::control_epoll_envent(int epoll_fd, int op, int fd, std::uint32_t events, void *data) {
        // This function is declared but not implemented.
        // The epoll control logic is directly in Listen() and HandleEpollEvent().
    }

} // snow