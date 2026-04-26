
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
            HTTPResponse response;
            bool allow_keep_alive;
            static const char* CRLF;
            static const size_t CRLF_SIZE = 2;
            static const char* COLON;

            const config::Config& config;
            struct ResolutionResult {
                HTTPResponseType::Type type;
                HTTPStatusCode status_code;
                std::string path;
                std::string reason;
                
                ResolutionResult(): status_code(NONE) {}
                ResolutionResult( HTTPStatusCode code, std::string r )
                    : status_code(code), reason(r) {}
            };

        private:
            std::map<std::string, std::string>::iterator current_header; /* track the current header */
            std::string line_buff;

        public:
            enum SerializeState {
                RESPONSE_LINE,
                HEADERS,
                BODY,
                DONE
            } serialize_state;

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
        
        private:
            ::size_t serialize_headers( char* buff, ::size_t max_size );
            void serialize_current_header( void );

        public:
            std::string generate_directory_list( const char* dir );
            std::string generate_anchor( const char* name );

        public:
            HTTPDispatcher( const config::Config& conf );
            ~HTTPDispatcher();
            void build_error_response( HTTPStatusCode code, std::string reason );
            HandlerResult handle_request( const HTTPRequest& req );
            ::ssize_t produce( char* buff, ::size_t max_size );
            bool allow_presistance( void ) { return allow_keep_alive; };
            
    };
}
