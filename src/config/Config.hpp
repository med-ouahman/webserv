#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace config {

struct ListenEndPoint {
    uint32_t host;
    uint16_t port;
};

struct ReturnDirective {
    int return_code;
    std::string return_target;

    ReturnDirective()
        : return_code(0),
          return_target()
    {}
};

struct LocationConfig {
    std::string path;
    std::string root;
    std::vector<std::string> index;
    std::set<std::string> allowed_methods;
    ReturnDirective redirect;
    bool autoindex;
    bool upload_enabled;
    std::string upload_path;
    // std::string cgi_extension; // deprecated
    // std::string cgi_path; // deprecated
    std::map<std::string, std::string> cgi_pass;
    std::string cgi_dir; // where scripts, executables live
    size_t cgi_timeout;

    LocationConfig()
        : path(),
          root(),
          index(),
          allowed_methods(),
          redirect(),
          autoindex(false),
          upload_enabled(false),
          upload_path(),
        //   cgi_extension(),
        //   cgi_path(),
          cgi_pass(),
          cgi_dir(),
          cgi_timeout(0)
    {}
};

struct ServerConfig {
    std::vector<LocationConfig> locations;
    std::vector<ListenEndPoint> listens;
    std::vector<std::string> server_names;
    std::string root;
    std::vector<std::string> index_files;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;
    bool session_enabled;
    std::string session_cookie_name;
    size_t session_timeout;
    std::string session_store;

    ServerConfig()
        : locations(),
          listens(),
          server_names(),
          root(),
          index_files(),
          error_pages(),
          client_max_body_size(0),
          session_enabled(false),
          session_cookie_name(),
          session_timeout(0),
          session_store()
    {}
};

struct Config {
    std::vector<ServerConfig> servers;

    Config()
        : servers()
    {}

};

}
