#include "Uri.h"
#include <cstdint>
#include <sstream>
#include <string>

namespace snow {
    Uri::Uri(const std::string &uri_str) {
        std::istringstream ss(uri_str);
        std::string temp;
        //1. 解析scheme
        if (std::getline(ss, temp, ':')) {
            scheme_ = temp;
            //跳过"//"
            if (ss.peek() == '/') ss.get();
            if (ss.peek() == '/') ss.get();
        } else {
            throw std::invalid_argument("Invalid URI: Missing scheme");
        }
        //2. 解析host和port
        if (std::getline(ss, temp, '/')) {
            auto pos = temp.find(':');
            if (pos != std::string::npos) {
                host_ = temp.substr(0, pos);
                port_ = static_cast<uint16_t>(std::stoi(temp.substr(pos + 1)));
            } else {
                host_ = temp;
                //目前只支持http
                if (scheme_ == "http") {
                    port_ = 80;
                } else {
                    port_ = 0;//未知
                }
            }
        }
        //3.解析path
        if (ss.rdbuf()->in_avail() > 0) {
            std::getline(ss, path_);
            path_ = "/" + path_;
        } else {
            path_ = "/";
        }
    }


// // 测试
// int main() {
//     Uri uri("http://example.com:8080/path/to/resource");

//     std::cout << "Scheme: " << uri.getScheme() << "\n";
//     std::cout << "Host: " << uri.getHost() << "\n";
//     std::cout << "Port: " << uri.getPort() << "\n";
//     std::cout << "Path: " << uri.getPath() << "\n";
//     return 0;
// }
}
