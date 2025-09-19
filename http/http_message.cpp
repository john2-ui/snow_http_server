#include "http_message.h"

namespace snow {
    std::string HttpUtility::to_string(HttpVersion version) {
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

    std::string HttpUtility::to_string(HttpMethod method) {
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

    std::string HttpUtility::to_string(HttpStatusCode code) {
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
    else return std::invalid_argument("Invalid HTTP method");
}

HttpVersion HttpUtility::string_to_version(std::string version) {
    std::string version_uppercase;
    std::transform(version.begin(), version.end(), std::back_inserter(version_uppercase),
                   [](char c) { return toupper(c); });
    if (version_uppercase == "HTTP/0.9")
        if (version_uppercase == "HTTP/0.9")
            return HttpVersion::HTTP_0_9;
        else if (version_uppercase == "HTTP/1.0")
            return HttpVersion::HTTP_1_0;
        else if (version_uppercase == "HTTP/1.1")
            return HttpVersion::HTTP_1_1;
        else if (version_uppercase == "HTTP/2.0")
            return HttpVersion::HTTP_2_0;
        else return std::invalid_argument("Invalid HTTP version");
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
            return std::invalid_argument("Invalid HTTP status code");
    }

    std::string HttpRequestToString(HttpRequest &request){

    }
    std::string HttpResponseToString(HttpResponse &response, bool sent_content = true){

    }
    HttpRequest StringToHttpRequest(const std::string &request_string){

    }
    HttpResponse StringToHttpResponse(const std::string &response_string){

    }
}