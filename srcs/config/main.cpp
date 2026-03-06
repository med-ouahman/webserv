#include <iostream>
#include "ConfigParser.hpp"

int main()
{
    try
    {
        config::ConfigParser parser;

        // Parse your config file
        config::Config conf = parser.parse("webserv.conf"); // replace with your config file path

        std::cout << "Number of servers: " << conf.servers.size() << std::endl;

        for (size_t i = 0; i < conf.servers.size(); ++i)
        {
            const config::ServerConfig& server = conf.servers[i];
            std::cout << "Server " << i+1 << ":" << std::endl;

            // Listen ports
            for (size_t j = 0; j < server.listens.size(); ++j)
                std::cout << "  Listen port: " << server.listens[j].port << std::endl;

            // Server names
            for (size_t j = 0; j < server.server_names.size(); ++j)
                std::cout << "  Server name: " << server.server_names[j] << std::endl;

            // Root
            std::cout << "  Root: " << server.root << std::endl;

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

                std::cout << "    Allowed methods: ";
                std::set<std::string>::const_iterator it;
                for (it = loc.allowed_methods.begin(); it != loc.allowed_methods.end(); ++it)
                    std::cout << *it << " ";
                std::cout << std::endl;

                if (!loc.redirect.return_target.empty())
                    std::cout << "    Return: " << loc.redirect.return_code 
                              << " -> " << loc.redirect.return_target << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Parser error: " << e.what() << std::endl;
    }

    return 0;
}