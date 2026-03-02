
#include "ConfigParser.hpp"
#include <iostream>

namespace config {
    ConfigParser::ConfigParser() {

    }
    
    ConfigParser::~ConfigParser() {

    }

    Config parse( const char* config_path ) {
        std::cout << config_path;
        return Config();
    }

    #ifdef DEV_MODE
    Config ConfigParser::build_default_config() {
        Config conf;
        ServerConfig server;
        LocationConfig root, images;
        server.client_max_body_size = 1000000;
        server.listens.push_back( (ListenEndPoint ) {
           0, 8082
        } );
        server.listens.push_back( (ListenEndPoint ) {
            0, 8081
        } );

        server.server_names.push_back("localhost");
        server.error_pages[404] = "./error_pages/404.html";
        server.error_pages[501] = "./error_pages/501.html";
        server.index_files.push_back("index.html");
        server.index_files.push_back("index.htm");
        root.root = server.root;
        root.allowed_methods.insert("GET");
        root.allowed_methods.insert("POST");
        server.locations.push_back(root);

        images.root = "./images";
        images.allowed_methods = root.allowed_methods;
        images.index = server.index_files;
        server.locations.push_back(images);
        conf.server = server;
        return conf;
    }
    #endif
}
