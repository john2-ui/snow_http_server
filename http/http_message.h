#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <map>
#include <string>
#include "Uri.h"
#include <cstdint>
#include <algorithm>
#include <stdexcept>
#include <sstream>

namespace snow {
    enum class HttpMethod {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH
    };

    enum class HttpVersion {
        HTTP_0_9 = 9,
        HTTP_1_0 = 10,
        HTTP_1_1 = 11,
        HTTP_2_0 = 20
    };

    enum class HttpStatusCode {
        Continue = 100,
        SwitchingProtocols = 101,
        EarlyHints = 103,
        Ok = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        NotModified = 304,
        BadRequest = 400,
        Unauthorized = 401,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        RequestTimeout = 408,
        ImATeapot = 418,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnvailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505
    };

    //包含处理enum classes和string之间转换的工具函数
    class HttpUtility {
    public:
        static std::string To_String(HttpVersion version);

        static std::string To_String(HttpMethod method);

        static std::string To_String(HttpStatusCode code);

        static HttpMethod string_to_method(std::string method);

        static HttpVersion string_to_version(std::string version);

        static HttpStatusCode string_to_code(uint32_t code);
    };

    //HttpRequest和HttpResponse的公共基类，包含两者都有的成员
    class HttpMessageInterface {
    public:
        HttpMessageInterface() : version_(HttpVersion::HTTP_1_1) {}

        virtual ~HttpMessageInterface() = default;

        //header methods
        void setHeader(const std::string &key, const std::string &value) {
            headers_[key] = std::move(value);
        }

        void removeHeader(const std::string &key) {
            headers_.erase(key);
        }

        void clearHeaders() {
            headers_.clear();
        }

        std::string getHeadersValue(const std::string &key) {
            if (headers_.count(key) > 0) return headers_.at(key);
            return std::string{};
        }

        std::map <std::string, std::string> getHeaders() const {
            return headers_;
        }

        //Content functions
        std::string getContent() const {
            return content_;
        }

        void setContent(const std::string &content) {
            content_ = std::move(content);
            setContentLength();
        }

        void clearContent() {
            content_.clear();
            setContentLength();
        }

        size_t getContent_length() const {
            return content_.length();
        }

        //version 
        HttpVersion getVersion() const {
            return version_;
        }

        void setVersion(HttpVersion version) {
            version_ = version;
        }

    protected:
        HttpVersion version_;                       //http版本号
        std::map <std::string, std::string> headers_;//头部
        std::string content_;                       //消息体

        //如果需要发送content，就需要设置content length
        void setContentLength() {
            setHeader("Content-Length", std::to_string(content_.length()));
        }
    };

    class HttpRequest : public HttpMessageInterface {
    public:
        HttpRequest() : method_(HttpMethod::GET) {}

        ~HttpRequest() = default;

        //set Method
        void setMethod(HttpMethod new_method) {
            method_ = new_method;
        }

        void setUri(Uri new_uri) {
            uri_ = std::move(new_uri);
        }

        //get method
        HttpMethod getMethod() const {
            return method_;
        }

        Uri getUri() const {
            return uri_;
        }

        //友元函数
        friend std::string HttpRequestToString(HttpRequest &request);

        friend HttpRequest StringToHttpRequest(const std::string &request_string);

    private:
        HttpMethod method_;
        Uri uri_;
    };

    class HttpResponse : public HttpMessageInterface {
    public:
        HttpResponse() : status_code_(HttpStatusCode::Ok) {}

        ~HttpResponse() = default;

        //set method
        void setStatusCode(HttpStatusCode code) {
            status_code_ = code;
        }

        //get method
        HttpStatusCode getStatusCode() const {
            return status_code_;
        }

        //友元函数
        friend std::string HttpResponseToString(HttpResponse &response, bool sent_content);//友元函数声明里面不能写默认参数
        friend HttpResponse StringToHttpResponse(const std::string &response_string);

    private:
        HttpStatusCode status_code_;
    };

    //Utility functions声明为类的友元函数是因为不希望把这个接口暴露在外，同时这些函数需要访问类的私有成员
    //用于把HttpRequest & HttpResponse和string之间相互转化
    //实现客户端和服务端之间收发消息的工具函数
    std::string HttpRequestToString(HttpRequest &request);
    std::string HttpResponseToString(HttpResponse &response, bool sent_content = true);//HttpResponse可以选择性发送内容返回
    HttpRequest StringToHttpRequest(const std::string &request_string);
    HttpResponse StringToHttpResponse(const std::string &response_string);
}
#endif // HTTP_MESSAGE_H