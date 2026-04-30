
#pragma once

#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"
#include <vector>
#include <iostream>

namespace config {
    struct ServerConfig;
    struct LocationConfig;
    struct Config;
}

namespace http {

    struct HTTPRequest;

    class HTTPResponseHandler {
        private:
            HTTPResponse response;
            bool allow_keep_alive;

            class HTTPResponseType {
                public:
                enum Type {

                    STATIC_FILE,
                    DIRECTORY,
                    CGI,
                    FILE_UPLOAD,
                    FILE_DELETE,
                    REDIRECT,
                    ERROR_RESPONSE
                };
            };
            
            const config::Config& config;
            struct ResolutionResult {
                HTTPResponseType::Type type;
                int status_code;
                std::string path;
                std::string reason;
                
                ResolutionResult(): status_code(0) {}
                ResolutionResult( int code, std::string r )
                    : status_code(code), reason(r) {}
            };
            
            ::size_t offset_; /* How much bytes are sent in the response.*/
            
        public:
            enum SerializeState {
                HEADERS,
                BODY,
                DONE
            } serialize_state;

        private:
            ResolutionResult resolve( const HTTPRequest& req, const config::ServerConfig& server );
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
            std::string extract_path( const std::string& url );
            static bool file_exists( const char* filename );

        public:
            std::string generate_directory_list( const char* dir );
            std::string generate_anchor( const char* name );

        public:
            HTTPResponseHandler( const config::Config& conf );
            ~HTTPResponseHandler();
            void serialize( void );
            void build_error_response( HTTPStatusCode code, std::string reason );
            void handle_request( const HTTPRequest& req );
            ::ssize_t produce( char* buff, ::size_t max_size );
            bool allow_presistance( void ) { return allow_keep_alive; };
            
    };
}
