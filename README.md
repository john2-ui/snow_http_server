⸻

HTTP Server 项目结构设计

📂 目录结构

src/
 ├── net/         # 网络层：epoll + socket
 ├── coroutine/   # 协程层：用户态调度
 ├── http/        # 协议层：HTTP 报文解析/生成
 ├── app/         # 应用层：路由 & Server 封装
 └── main.cpp     # 入口


⸻

1️⃣ net (网络层)

目标：统一抽象 I/O 事件和 TCP 连接，提供高效事件驱动（基于 epoll）。

主要功能
	•	封装非阻塞 TCP socket。
	•	epoll 事件循环。
	•	提供回调机制（连接、读、写、关闭）。

主要类
	•	EventLoop
	•	void Loop()
	•	void AddEvent(int fd, uint32_t events, Callback cb)
	•	void RemoveEvent(int fd)
	•	TcpServer
	•	void Start()
	•	void Stop()
	•	TcpConnection
	•	ssize_t Send(const std::string&)
	•	ssize_t Recv(std::string&)

⸻

2️⃣ coroutine (协程层)

目标：在单线程内调度大量协程，遇到 I/O 阻塞时挂起，事件到来时恢复。

主要功能
	•	封装用户态协程（上下文切换、栈管理）。
	•	调度器：管理就绪队列。
	•	I/O 管理器：结合 epoll，把 fd 事件和协程绑定。

主要类
	•	Coroutine
	•	void Resume()
	•	void Yield()
	•	状态：INIT / RUNNING / SUSPENDED / FINISHED
	•	Scheduler
	•	void Schedule(Coroutine* coro)
	•	void Run()
	•	IoManager
	•	void AddEvent(int fd, uint32_t events, Coroutine* coro)
	•	void IdleLoop() （基于 epoll_wait）

⸻

3️⃣ http (协议层)

目标：封装 HTTP 协议，提供请求/响应的对象表示。

主要功能
	•	请求解析：字符串 → HttpRequest。
	•	响应生成：HttpResponse → 字符串。
	•	常用工具函数：Method、Status、Version 转换。

主要类
	•	HttpRequest
	•	属性：method / uri / headers / body
	•	方法：static HttpRequest Parse(const std::string&)
	•	HttpResponse
	•	属性：status / headers / body
	•	方法：std::string ToString()

⸻

4️⃣ app (应用层)

目标：用户只写业务逻辑，不关心底层 I/O/协程/协议。

主要功能
	•	路由（URI + Method → 回调）。
	•	HttpServer：对外暴露统一 API，整合 net + coroutine + http。

主要类
	•	Router
	•	void AddRoute(const std::string& path, HttpMethod method, Handler cb)
	•	Handler Match(const std::string& path, HttpMethod method)
	•	HttpServer
	•	void RegisterHandler(path, method, handler)
	•	void Start()

⸻

5️⃣ main.cpp (入口)

目标：启动服务，注册路由，运行事件循环。

示例

#include "app/HttpServer.h"

int main() {
    simple_http::HttpServer server("0.0.0.0", 8080);

    server.RegisterHandler("/hello", HttpMethod::GET,
        [](const HttpRequest& req) {
            HttpResponse res(HttpStatusCode::Ok);
            res.SetContent("Hello, World!");
            return res;
        });

    server.Start();
    return 0;
}


⸻

🔗 分层依赖关系

main.cpp
   │
   ▼
 app (HttpServer, Router)
   │
   ▼
 http (HttpRequest/Response)
   │
   ▼
 net (TcpServer, EventLoop, TcpConnection)
   │
   ▼
 coroutine (Scheduler, Coroutine, IoManager)


⸻

要不要我再帮你写一个 开发顺序 roadmap（比如先写 http 层 → net → app → coroutine），这样你就知道按什么顺序逐步实现？