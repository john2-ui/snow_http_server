#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <map>
#include <string>

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
        static std::string to_string(HttpVersion version);
        static std::string to_string(HttpMethod method);
        static std::string to_string(HttpStatusCode code);
        static HttpMethod string_to_method(std::string method);
        static HttpVersion string_to_version(std::string version);
        static HttpStatusCode string_to_code(std::string code);
    };

    //HttpRequest和HttpResponse的公共基类，包含两者都有的成员
    class HttpMessageInterface {
        public:
            HttpMessageInterface() : version_(HttpVersion::HTTP_1_1){}
            virtual ~HttpMessageInterface() = default;

            //header methods
            void setHeader(const std::string & key, const std::string & value) {
                headers_[key] = std::move(value);
            }
            
            void removeHeader(const std::string & key){
                headers_.erase(key);
            }

            void clearHeaders() {
                headers_.clear();
            }
        protected:
            HttpVersion version_;                       //http版本号
            std::map<std::string, std::string> headers_;//头部
            std::string content_;                       //消息体
    };
}
#endif // HTTP_MESSAGE_H