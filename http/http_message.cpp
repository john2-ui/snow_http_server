#include "http_message.h"
#include <cctype>
#include <cstddef>
#include <sstream>
#include <stdexcept>

namespace snow {
    std::string HttpUtility::To_String(HttpVersion version) {
        switch (version) {
            case HttpVersion::HTTP_0_9:
                return "HTTP/0.9";
            case HttpVersion::HTTP_1_0:
                return "HTTP/1.0";
            case HttpVersion::HTTP_1_1:
                return "HTTP/1.1";
            case HttpVersion::HTTP_2_0:
                return "HTTP/2.0";
            default:
                return std::string{};
        }
    }

    std::string HttpUtility::To_String(HttpMethod method) {
        switch (method) {
            case HttpMethod::GET:
                return "GET";
            case HttpMethod::HEAD:
                return "HEAD";
            case HttpMethod::POST:
                return "POST";
            case HttpMethod::PUT:
                return "PUT";
            case HttpMethod::DELETE:
                return "DELETE";
            case HttpMethod::CONNECT:
                return "CONNECT";
            case HttpMethod::OPTIONS:
                return "OPTIONS";
            case HttpMethod::TRACE:
                return "TRACE";
            case HttpMethod::PATCH:
                return "PATCH";
            default:
                return std::string{};
        }
    }

    std::string HttpUtility::To_String(HttpStatusCode code) {
        switch (code) {
            case HttpStatusCode::Continue:
                return "Continue";
            case HttpStatusCode::SwitchingProtocols:
                return "SwitchingProtocols";
            case HttpStatusCode::EarlyHints:
                return "EarlyHints";
            case HttpStatusCode::Ok:
                return "Ok";
            case HttpStatusCode::Created:
                return "Created";
            case HttpStatusCode::Accepted:
                return "Accepted";
            case HttpStatusCode::NonAuthoritativeInformation:
                return "NonAuthoritativeInformation";
            case HttpStatusCode::NoContent:
                return "NoContent";
            case HttpStatusCode::ResetContent:
                return "ResetContent";
            case HttpStatusCode::PartialContent:
                return "PartialContent";
            case HttpStatusCode::MultipleChoices:
                return "MultipleChoices";
            case HttpStatusCode::MovedPermanently:
                return "MovedPermanently";
            case HttpStatusCode::Found:
                return "Found";
            case HttpStatusCode::NotModified:
                return "NotModified";
            case HttpStatusCode::BadRequest:
                return "BadRequest";
            case HttpStatusCode::Unauthorized:
                return "Unauthorized";
            case HttpStatusCode::Forbidden:
                return "Forbidden";
            case HttpStatusCode::NotFound:
                return "NotFound";
            case HttpStatusCode::MethodNotAllowed:
                return "MethodNotAllowed";
            case HttpStatusCode::RequestTimeout:
                return "RequestTimeout";
            case HttpStatusCode::ImATeapot:
                return "ImATeapot";
            case HttpStatusCode::InternalServerError:
                return "InternalServerError";
            case HttpStatusCode::NotImplemented:
                return "NotImplemented";
            case HttpStatusCode::BadGateway:
                return "BadGateway";
            case HttpStatusCode::ServiceUnvailable:
                return "ServiceUnvailable";
            case HttpStatusCode::GatewayTimeout:
                return "GatewayTimeout";
            case HttpStatusCode::HttpVersionNotSupported:
                return "HttpVersionNotSupported";
            default:
                return std::string{};
        }

    }

HttpMethod HttpUtility::string_to_method(std::string method) {
    std::string method_uppercase;
    std::transform(method.begin(), method.end(), std::back_inserter(method_uppercase),
                   [](char c) { return toupper(c); });

    if (method_uppercase == "GET")
        return HttpMethod::GET;
    else if (method_uppercase == "HEAD")
        return HttpMethod::HEAD;
    else if (method_uppercase == "POST")
        return HttpMethod::POST;
    else if (method_uppercase == "PUT")
        return HttpMethod::PUT;
    else if (method_uppercase == "DELETE")
        return HttpMethod::DELETE;
    else if (method_uppercase == "CONNECT")
        return HttpMethod::CONNECT;
    else if (method_uppercase == "OPTIONS")
        return HttpMethod::OPTIONS;
    else if (method_uppercase == "TRACE")
        return HttpMethod::TRACE;
    else if (method_uppercase == "PATCH")
        return HttpMethod::PATCH;
    else throw std::invalid_argument("Invalid HTTP method");
}

HttpVersion HttpUtility::string_to_version(std::string version) {
    std::string version_uppercase;
    std::transform(version.begin(), version.end(), std::back_inserter(version_uppercase),
                   [](char c) { return toupper(c); });
        if (version_uppercase == "HTTP/0.9")
            return HttpVersion::HTTP_0_9;
        else if (version_uppercase == "HTTP/1.0")
            return HttpVersion::HTTP_1_0;
        else if (version_uppercase == "HTTP/1.1")
            return HttpVersion::HTTP_1_1;
        else if (version_uppercase == "HTTP/2.0")
            return HttpVersion::HTTP_2_0;
        else throw std::invalid_argument("Invalid HTTP version");
}

HttpStatusCode HttpUtility::string_to_code(uint32_t code) {
    switch (code) {
        case 100:
            return HttpStatusCode::Continue;
        case 101:
            return HttpStatusCode::SwitchingProtocols;
        case 103:
            return HttpStatusCode::EarlyHints;
        case 200:
            return HttpStatusCode::Ok;
        case 201:
            return HttpStatusCode::Created;
        case 202:
            return HttpStatusCode::Accepted;
        case 203:
            return HttpStatusCode::NonAuthoritativeInformation;
        case 204:
            return HttpStatusCode::NoContent;
        case 205:
            return HttpStatusCode::ResetContent;
        case 206:
            return HttpStatusCode::PartialContent;
        case 300:
            return HttpStatusCode::MultipleChoices;
        case 301:
            return HttpStatusCode::MovedPermanently;
        case 302:
            return HttpStatusCode::Found;
        case 304:
            return HttpStatusCode::NotModified;
        case 400:
            return HttpStatusCode::BadRequest;
        case 401:
            return HttpStatusCode::Unauthorized;
        case 403:
            return HttpStatusCode::Forbidden;
        case 404:
            return HttpStatusCode::NotFound;
        case 405:
            return HttpStatusCode::MethodNotAllowed;
        case 408:
            return HttpStatusCode::RequestTimeout;
        case 418:
            return HttpStatusCode::ImATeapot;
        case 500:
            return HttpStatusCode::InternalServerError;
        case 501:
            return HttpStatusCode::NotImplemented;
        case 502:
            return HttpStatusCode::BadGateway;
        case 503:
            return HttpStatusCode::ServiceUnvailable;
        case 504:
            return HttpStatusCode::GatewayTimeout;
        case 505:
            return HttpStatusCode::HttpVersionNotSupported;
        default:
            throw std::invalid_argument("Invalid HTTP status code");
    }
}

    //使用场景：客户端发送HttpRequest给服务器时，需要转换为string发送
    //服务器接收到string后需要转换为HttpRequest进行处理
    std::string HttpRequestToString(HttpRequest &request){
        std::ostringstream request_stream;
        //请求行
        request_stream << snow::HttpUtility::To_String(request.getMethod()) << " ";
        request_stream << request.getUri().getPath() << " ";
        request_stream << snow::HttpUtility::To_String(request.getVersion()) << "\r\n";
        //请求头
        for (const auto & header : request.headers_){
            request_stream << header.first << ": " << header.second << "\r\n";
        }
        request_stream << "\r\n";
        //请求体
        if(!request.content_.empty())
            request_stream << request.content_;

        return request_stream.str();
    }
    HttpRequest StringToHttpRequest(const std::string &request_string){
        HttpRequest req;
        std::istringstream iss;//用于解析
        std::string start_line, headers_line, message_body;
        std::string method, path, version;
        std::string key, value;
        size_t lpos = 0, rpos = 0;


        //获取请求行
        rpos = request_string.find("\r\n", lpos);
        if (rpos == std::string::npos) {
            throw std::invalid_argument("Could not find the rquest start line");
        }

        start_line=request_string.substr(lpos, rpos - lpos);
        //获取完请求行之后移动左指针
        lpos = rpos + 2;
        //获取headers
        rpos = request_string.find("\r\n\r\n", lpos);//找到headers结束位置
        if (rpos != std::string::npos) {
            headers_line = request_string.substr(lpos, rpos - lpos);
            lpos  = rpos + 4;
            rpos = request_string.length();
            //解析body
            if (lpos <  rpos) {
                message_body = request_string.substr(lpos, rpos - lpos);
            }
        }

        //解析请求行
        iss.clear();
        iss.str(start_line);
        iss >> method >> path >> version;
        if (iss.fail()){
            throw std::invalid_argument("Invalid start line format");
        }
        req.setMethod(HttpUtility::string_to_method(method));
        Uri uri;
        uri.setPath(path);
        req.setUri(uri);
        if (HttpUtility::string_to_version(version) != req.getVersion()){
            throw std::logic_error("HTTP version not supported");//目前只支持HTTP1.1
        }

        //解析请求头
        iss.clear();
        std::string line;
        iss.str(headers_line);
        while (std::getline(iss, line)){
            std::istringstream header_stream(line);
            std::getline(header_stream, key, ':');
            std::getline(header_stream, value);

            //删除空格（注意：只删除头尾的）
            auto trim = [](std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch){ return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
};
        trim(key);
        trim(value);
        if (!key.empty()){
            req.setHeader(key,value);
        }
        }

        req.setContent(message_body);

        return req;
    }

    //使用场景：服务器发送HttpResponse给客户端时，需要转换为string发送
    //客户端接收到string后需要转换为HttpResponse进行处理
    std::string HttpResponseToString(HttpResponse &response, bool send_content) {
    std::ostringstream response_stream;

    // 如果要发送 body，保证 Content-Length 存在
    if (send_content && !response.getContent().empty()) {
        response.setContentLength();  // 更新 headers map
    }

    // 写状态行
    response_stream << HttpUtility::To_String(response.getVersion()) << " "
                    << static_cast<int>(response.getStatusCode()) << " "
                    << HttpUtility::To_String(response.getStatusCode()) << "\r\n";

    // 写 headers
    for (const auto &header : response.getHeaders()) {
        response_stream << header.first << ": " << header.second << "\r\n";
    }
    response_stream << "\r\n";

    // 写 body
    if (send_content && !response.getContent().empty()) {
        response_stream << response.getContent();
    }

    return response_stream.str();
    }

    HttpResponse StringToHttpResponse(const std::string &response_string){
        HttpResponse rep;
        std::istringstream iss;
        std::string status_line, headers_line, message_body;
        std::string version, status_code, reason_phrase;
        std::string key, value;
        size_t lpos = 0, rpos = 0;


        //删除空格的辅助函数（注意：只删除头尾的）
            auto trim = [](std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch){ return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
    };

        //获取状态行(status line)
        rpos = response_string.find("\r\n", lpos);
        if (rpos == std::string::npos){
            throw std::invalid_argument("Could not find the status line");
        }
        status_line = response_string.substr(lpos, rpos- lpos);
        lpos = rpos +  2;
        //获取响应头
        rpos = response_string.find("\r\n\r\n", lpos);
        if (rpos != std::string::npos) {
            headers_line = response_string.substr(lpos, rpos - lpos);
            lpos = rpos + 4;
            rpos = response_string.length();
            //获取body
            if (lpos < rpos){
                message_body = response_string.substr(lpos, rpos - lpos);
            }
        }

        //解析状态行
        iss.clear();
        iss.str(status_line);
        iss >> version >> status_code;
        std::getline(iss, reason_phrase);   //状态原因可能包含空格，要使用getline

        //reason_phrase这里暂时不需要，可以使用status_code直接查找
        //如果将来需要自定义错误，那么就需要加入reason_phrase

        if (iss.fail()) {
            throw std::invalid_argument("Invalid response status line format");
        }
        if (HttpUtility::string_to_version(version) != rep.getVersion()){
            throw std::logic_error("HTTP version not supported");//目前只支持HTTP1.1
        }
        rep.setStatusCode(HttpUtility::string_to_code(stoi(status_code)));

        //解析请求头
        iss.clear();
        std::string line;
        iss.str(headers_line);
        while (std::getline(iss, line)){
            std::istringstream header_stream(line);
            std::getline(header_stream, key, ':');
            std::getline(header_stream, value);

            //删除空格（注意：只删除头尾的）
            auto trim = [](std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch){ return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());
};
        trim(key);
        trim(value);
        if (!key.empty()){
            rep.setHeader(key,value);
        }
        }

        rep.setContent(message_body);
        return rep;
    }

}