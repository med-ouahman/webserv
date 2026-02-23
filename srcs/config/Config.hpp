
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
    };

    struct LocationConfig {
        std::string path;
        std::string root;
        std::vector<std::string> index;
        std::set<std::string> allowed_methods;
        ReturnDirective redirect;
    };

    struct ServerConfig {
        std::vector<LocationConfig> locations;
        std::vector<ListenEndPoint> listens;
        std::vector<std::string> server_names;
        std::string root;
        std::vector<std::string> index_files;
        std::map<int, std::string> error_pages;
        std::map<std::string, std::string> cgi_pass;
        size_t client_max_body_size;
    };

    struct Config {
		ServerConfig server;
	};
}
