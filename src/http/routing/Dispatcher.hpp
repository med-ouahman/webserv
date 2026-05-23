
#pragma once

#include <vector>
#include <iostream>
#include <memory>
#include <stdint.h>
#include "Config.hpp"
#include "Response.hpp"
#include "Response.hpp"

namespace http {

    class Request;
    class IRequestHandler;

    struct CGIRequestContext {
        std::string temp_body_path;

        std::string script_filename;      
        std::string interpreter_path;     
        std::string path_info;
        std::string script_name;
        std::string working_directory;

        std::string server_name;
        uint32_t    server_port;
        time_t      timeout_seconds;
    };

    struct ResolutionResult {
        
        ResponseType type;
        StatusCode status_code;
        
        std::string mime_type;
        std::string path;
        const Request& request;
        const config::LocationConfig* location;
            
        ResolutionResult( const Request& r): request(r) {}
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
            static bool allow_presistance() { return true; };
            static IRequestHandler* create_request_handler( const ResolutionResult& result );
            static CGIRequestContext resolve_cgi_context( const ResolutionResult& result );
            static size_t file_size( const std::string & filename );
    };
}
