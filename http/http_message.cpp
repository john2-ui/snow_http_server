#include "http_message.h"

namespace snow {
    std::string HttpUtility::to_string(HttpVersion version){}
    std::string HttpUtility::to_string(HttpMethod method) {}
    std::string HttpUtility::to_string(HttpStatusCode code){}
    
    HttpMethod string_to_method(std::string method){}
    HttpVersion string_to_version(std::string version){}
    HttpStatusCode string_to_code(std::string code){}
}