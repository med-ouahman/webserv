
#pragma once

#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"
#include <vector>
#include <iostream>
#include "CGIContext.hpp"
#include <memory>

namespace config {
    struct ServerConfig;
    struct LocationConfig;
    struct Config;
}

namespace http {

    struct HTTPRequest;

    struct ResolutionResult {
        HTTPResponseType::Type type;
        HTTPStatusCode status_code;
        std::string reason;
        std::string mime_type;
        std::map<std::string, std::string> extra_headers;
        BodyType::Type body_type;
        BodyType b;
        BodyStorage::Type body_storage;
        bool close_conn;
        size_t max_body_size;
        std::string path;
        CGIContext* cgi_context;
    };


    class HTTPDispatcher {
        private:
            const config::Config& config;
            bool allow_keep_alive;

        private:
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
            std::string extract_path( const std::string& url );
            static bool file_exists( const char* filename );

        public:
            std::string generate_directory_list( const char* dir ); // TO BE MOVED
            std::string generate_anchor( const char* name ); // TO BE MOVED
            void build_error_response( HTTPStatusCode code, std::string reason ); // TO BE MOVED;
            ResolutionResult resolve( const HTTPRequest& req, const config::ServerConfig& server );
            HTTPDispatcher( const config::Config& conf );
            ~HTTPDispatcher();
            bool allow_presistance() { return allow_keep_alive; };
            void handle_request( ResolutionResult& r, const HTTPRequest& req );
            CGIContext get_cgi_context( const HTTPRequest& req, const ResolutionResult& result );

    };
}
