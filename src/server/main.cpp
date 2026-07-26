
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"
#include "sys.h"

#include <iostream>
#include <iomanip>

namespace config {

static std::string indent(int level) {
    return std::string(level * 4, ' ');
}

static void printVector(const std::vector<std::string>& vec, int level) {
    if (vec.empty()) {
        std::cout << "[]";
        return;
    }

    std::cout << "[\n";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << indent(level + 1) << vec[i];
        if (i + 1 != vec.size())
            std::cout << ",";
        std::cout << "\n";
    }
    std::cout << indent(level) << "]";
}
static void printSet(const std::set<std::string>& s, int level) {
    if (s.empty()) {
        std::cout << "{}";
        return;
    }

    std::cout << "{\n";
    for (std::set<std::string>::const_iterator it = s.begin();
         it != s.end(); ++it) {
        std::cout << indent(level + 1) << *it << "\n";
    }
    std::cout << indent(level) << "}";
}
static void printMap(const std::map<std::string, std::string>& m, int level) {
    if (m.empty()) {
        std::cout << "{}";
        return;
    }

    std::cout << "{\n";
    for (std::map<std::string, std::string>::const_iterator it = m.begin();
         it != m.end(); ++it) {
        std::cout << indent(level + 1)
                  << it->first << " -> " << it->second << "\n";
    }
    std::cout << indent(level) << "}";
}

static void printErrorPages(const std::map<int, std::string>& m, int level) {
    if (m.empty()) {
        std::cout << "{}";
        return;
    }

    std::cout << "{\n";
    for (std::map<int, std::string>::const_iterator it = m.begin();
         it != m.end(); ++it) {
        std::cout << indent(level + 1)
                  << it->first << " -> " << it->second << "\n";
    }
    std::cout << indent(level) << "}";
}

void printConfig(const Config& config) {
    std::cout << "=====================================================\n";
    std::cout << "                    CONFIGURATION\n";
    std::cout << "=====================================================\n\n";

    for (size_t s = 0; s < config.servers.size(); ++s) {
        const ServerConfig& server = config.servers[s];

        std::cout << "Server #" << s + 1 << "\n";
        std::cout << "-----------------------------------------------------\n";

        std::cout << "Root                  : " << server.root << "\n";

        std::cout << "Server Names          : ";
        printVector(server.server_names, 0);
        std::cout << "\n";

        std::cout << "Index Files           : ";
        printVector(server.index_files, 0);
        std::cout << "\n";

        std::cout << "Client Max Body Size  : "
                  << server.client_max_body_size << "\n";

        std::cout << "Session Enabled       : "
                  << (server.session_enabled ? "true" : "false") << "\n";

        std::cout << "Session Cookie        : "
                  << server.session_cookie_name << "\n";

        std::cout << "Session Timeout       : "
                  << server.session_timeout << "\n";

        std::cout << "Session Store         : "
                  << server.session_store << "\n";

        std::cout << "\nListen Endpoints:\n";
        for (size_t i = 0; i < server.listens.size(); ++i) {
            std::cout << "    [" << i << "] "
                      << server.listens[i].host
                      << ":" << server.listens[i].port << "\n";
        }

        std::cout << "\nError Pages:\n    ";
        printErrorPages(server.error_pages, 1);
        std::cout << "\n";

        std::cout << "\nLocations (" << server.locations.size() << ")\n";

        for (size_t l = 0; l < server.locations.size(); ++l) {
            const LocationConfig& loc = server.locations[l];

            std::cout << "\n"
                      << indent(1)
                      << "Location #" << l + 1
                      << " (" << loc.path << ")\n";

            std::cout << indent(1)
                      << "-----------------------------------------\n";

            std::cout << indent(2)
                      << "Root            : " << loc.root << "\n";

            std::cout << indent(2)
                      << "Index           : ";
            printVector(loc.index, 2);
            std::cout << "\n";

            std::cout << indent(2)
                      << "Methods         : ";
            printSet(loc.allowed_methods, 2);
            std::cout << "\n";

            std::cout << indent(2)
                      << "Autoindex       : "
                      << (loc.autoindex ? "true" : "false") << "\n";

            std::cout << indent(2)
                      << "Upload Enabled  : "
                      << (loc.upload_enabled ? "true" : "false") << "\n";

            std::cout << indent(2)
                      << "Upload Path     : "
                      << loc.upload_path << "\n";

            std::cout << indent(2)
                      << "Redirect        : "
                      << loc.redirect.return_code
                      << " -> "
                      << loc.redirect.return_target
                      << "\n";

            // std::cout << indent(2)
            //           << "CGI Extension   : "
            //           << loc.cgi_extension << "\n";

            // std::cout << indent(2)
            //           << "CGI Path        : "
            //           << loc.cgi_path << "\n";

            std::cout << indent(2)
                      << "CGI Directory   : "
                      << loc.cgi_dir << "\n";

            std::cout << indent(2)
                      << "CGI Timeout     : "
                      << loc.cgi_timeout << "\n";

            std::cout << indent(2)
                      << "CGI Pass        : ";
            printMap(loc.cgi_pass, 2);
            std::cout << "\n";
        }

        std::cout << "\n";
    }

    std::cout << "=====================================================\n";
}

} // namespace config

int main(int argc, const char* argv[]) {

    (void)argc;

    sys::handle_signals();

    config::Config conf;
    
    {
        config::ConfigParser parser;
        if (None != parser.parse(conf, argv[1] != NULL ? argv[1]: "config/default.conf")) return ConfError;
    }

    return Server(conf).start();
}
