
#pragma once

#include "HTTPResponse.hpp"
#include "HTTPStatusCode.hpp"
#include <vector>


namespace config {
    struct ServerConfig;
    struct LocationConfig;
}

namespace http {

    struct HTTPRequest;

    class HTTPResponseHandler {
        private:
            HTTPResponse response;
            bool allow_keep_alive;
            enum HTTPResponseType {
                STATIC_FILE,
                DIRECTORY,
                CGI,
                FILE_UPLOAD,
                FILE_DELETE,
                REDIRECT,
                ERROR_RESPONSE
            };
            
            struct ResolutionResult {
                HTTPResponseType type;
                int status_code;
                std::string path;
            };
            
            ::size_t offset_; /* How much bytes are sent in the response.*/
            
        public:
            enum SerializeState {
                HEADERS,
                BODY,
                DONE
            } serialize_state;

        private:
            static ResolutionResult resolve( const HTTPRequest& req, const config::ServerConfig& server );
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
        public:
            HTTPResponseHandler();
            ~HTTPResponseHandler();
            void serialize( void );
            void build_error_response( HTTPStatusCode code, std::string reason );
            void handle_request( const HTTPRequest& req );
            ::ssize_t produce( char* buff, ::size_t max_size );
            bool allow_presistance( void ) { return allow_keep_alive; };
            
    };
}
