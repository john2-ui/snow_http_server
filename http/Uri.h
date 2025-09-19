//Uniform Resource Identifier

//http://example.com:8080/path/to/page?name=tom#section1
//\__/   \_________/ \__/\________/ \________/ \_______/
// |         |        |      |           |         |
//scheme   host     port   path       query     fragment


#ifndef URI_H
#define URI_H

#include <cctype>
#include <cstdint>
#include <string>
#include <algorithm>

class Uri{
public:
    explicit Uri(const std::string & uri_str);
    Uri() = default;
    ~Uri() = default;

    //get function
    std::string getScheme() const {return scheme_;}
    std::string getHost() const {return host_;}
    std::uint16_t getPort() const {return port_;}
    std::string getPath() const {return path_;}
    std::string getQuery() const {return query_;}
    std::string getFragment() const {return fragment_;}

private:
    std::string scheme_;
    std::string host_;
    std::uint16_t port_;
    std::string path_;
    std::string query_;
    std::string fragment_;
};

//Uri个个部分规范工具类
class UriNormalizer{
public:
    //强制小写化scheme和host(RFC 3986)
    static void NormalizeSchemeHost(std::string & scheme, std::string & host) {
        ToLower(scheme);
        ToLower(host);
    }

    //可选小写化，默认不转化
    //path默认不转化，query一般保留大小写
    static void NormalizePath(std::string & path, bool to_lower = false) {
        if(to_lower) {
            ToLower(path);
        }
    }
    static void NormalizeQuery(std::string & query, bool to_lower = false) {
        if(to_lower) {
            ToLower(query);
        }
    }
private:
    static void ToLower(std::string & s) {
        std::transform(s.begin(), s.end(), s.begin(), 
        [](unsigned char c) {return std::tolower(c);});
    }
};

#endif