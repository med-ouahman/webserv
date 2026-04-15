#include <arpa/inet.h>
#include <iostream>
#include <set>
#include "ConfigParser.hpp"

int main(int argc, char** argv)
{
    try
    {
        const char* configPath = "webserv.conf";
        if (argc > 1)
            configPath = argv[1];

        config::ConfigParser parser;
        config::Config conf = parser.parse(configPath);

        std::cout << "Number of servers: " << conf.servers.size() << std::endl;

        for (size_t i = 0; i < conf.servers.size(); ++i)
        {
            const config::ServerConfig& server = conf.servers[i];
            std::cout << "Server " << i+1 << ":" << std::endl;

            for (size_t j = 0; j < server.listens.size(); ++j)
            {
                std::cout << "  Listen: " << server.listens[j].port << " ";
                struct in_addr addr;
                addr.s_addr = server.listens[j].host;
                std::cout << "host=" << inet_ntoa(addr) << std::endl;
            }

            // Server names
            for (size_t j = 0; j < server.server_names.size(); ++j)
                std::cout << "  Server name: " << server.server_names[j] << std::endl;

            // Root
            std::cout << "  Root: " << server.root << std::endl;
            std::cout << "  Client max body size: " << server.client_max_body_size << std::endl;
            std::cout << "  Session enabled: " << (server.session_enabled ? "yes" : "no") << std::endl;
            std::cout << "  Session cookie name: " << server.session_cookie_name << std::endl;
            std::cout << "  Session timeout: " << server.session_timeout << std::endl;
            std::cout << "  Session store: " << server.session_store << std::endl;

            // Index files
            std::cout << "  Index files: ";
            for (size_t j = 0; j < server.index_files.size(); ++j)
                std::cout << server.index_files[j] << " ";
            std::cout << std::endl;

            // Locations
            for (size_t j = 0; j < server.locations.size(); ++j)
            {
                const config::LocationConfig& loc = server.locations[j];
                std::cout << "  Location " << loc.path << ":" << std::endl;
                std::cout << "    Root: " << loc.root << std::endl;
                std::cout << "    Autoindex: " << (loc.autoindex ? "on" : "off") << std::endl;
                std::cout << "    Upload enabled: " << (loc.upload_enabled ? "yes" : "no") << std::endl;
                std::cout << "    Upload path: " << loc.upload_path << std::endl;

                std::cout << "    Index files: ";
                for (size_t k = 0; k < loc.index.size(); ++k)
                    std::cout << loc.index[k] << " ";
                std::cout << std::endl;

                std::cout << "    Allowed methods: ";
                for (std::set<std::string>::const_iterator it = loc.allowed_methods.begin(); it != loc.allowed_methods.end(); ++it)
                    std::cout << *it << " ";
                std::cout << std::endl;

                if (!loc.redirect.return_target.empty())
                    std::cout << "    Return: " << loc.redirect.return_code << " -> " << loc.redirect.return_target << std::endl;

                for (std::map<std::string, std::string>::const_iterator it = loc.cgi_pass.begin(); it != loc.cgi_pass.end(); ++it)
                    std::cout << "    CGI pass " << it->first << " => " << it->second << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Parser error: " << e.what() << std::endl;
    }

    return 0;
}