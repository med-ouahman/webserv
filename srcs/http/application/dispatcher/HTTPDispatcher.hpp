
#pragma once

#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"
#include <vector>
#include <iostream>
#include "CGIContext.hpp"

namespace config {
    struct ServerConfig;
    struct LocationConfig;
    struct Config;
}

namespace http {

    struct HTTPRequest;

    class HTTPDispatcher {

        private:
            const config::Config& config;
            bool allow_keep_alive;

            struct ResolutionResult {
                HTTPResponseType::Type type;
                HTTPStatusCode status_code;
                std::string path;
                std::string reason;
                
                ResolutionResult(): status_code(NONE) {}
                ResolutionResult( HTTPStatusCode code, std::string r )
                    : status_code(code), reason(r) {}
            };

        public:
            struct HandlerResult {
                HTTPResponseType::Type response_type;
                CGIContext cgi_ctx;
            };

        private:
            ResolutionResult resolve( const HTTPRequest& req, const config::ServerConfig& server );
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
            std::string extract_path( const std::string& url );
            static bool file_exists( const char* filename );
            CGIContext get_cgi_context( const HTTPRequest& req, const ResolutionResult& result );

        public:
            std::string generate_directory_list( const char* dir );
            std::string generate_anchor( const char* name );
            void build_error_response( HTTPStatusCode code, std::string reason );

        public:
            HTTPDispatcher( const config::Config& conf );
            ~HTTPDispatcher();
            HandlerResult handle_request( const HTTPRequest& req );
            bool allow_presistance() { return allow_keep_alive; };
            
    };
}
