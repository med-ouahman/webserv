
#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <stdint.h>
#include "Response.hpp"
#include "BodyHandler.hpp"

namespace core {
    class Connection;
}

namespace config {
    struct ServerConfig;
    struct LocationConfig;
    struct Config;
}

namespace http {

    struct Request;
    class IRequestHandler;

    struct CGIContext {
        std::string temp_body_path;
        std::string script_filename;      
        std::string interpreter_path;     
        std::string path_info;
        std::string script_name;          
        std::string working_directory;
        std::string server_name;          
        uint32_t timeout_seconds;     
        uint16_t server_port;       
    };

    struct ResolutionResult {
        
        ResponseType::Type type;
        StatusCode status_code;
        
        std::string mime_type;
        std::string path;
        const Request& request;
        const config::LocationConfig* location;
            
        ResolutionResult( const Request& r): request(r) {}
    };

    struct BodyConf {
        BodyType::Type type;
        BodyStorage::Type storage;
        size_t parsed_body_size;
        std::string path;
    };


    class Dispatcher {
        public:
            static const std::size_t MAX_BUFFERED_BODY_SIZE = 1024;

        private:
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
            static std::string extract_path( const std::string& url );
            static bool file_exists( const char* filename );

        public:
            static std::string generate_directory_list( const char* dir ); // TO BE MOVED
            static std::string generate_anchor( const char* name ); // TO BE MOVED
        
            static ResolutionResult resolve( const Request& req );
            Dispatcher();
            ~Dispatcher();
            static bool allow_presistance() { return true; };
            static IRequestHandler* create_request_handler( core::Connection& conn, const ResolutionResult& result );
            static CGIContext resolve_cgi_context( const ResolutionResult& result );
			static BodyType::Type detect_body_type( const Request& request );
            static BodyConf configure_body( const Request& request, const ResolutionResult& result );
    };
}
