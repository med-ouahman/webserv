
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
        public:
            enum SerializeState {
                HEADERS,
                BODY,
                DONE
            } serialize_state;
        private:
            ResolutionResult resolve( const HTTPRequest& req, const config::ServerConfig& server ) const;
            static const config::LocationConfig* find_location( const std::string& url,
                const std::vector<config::LocationConfig>& locations );
        public:
            HTTPResponseHandler();
            ~HTTPResponseHandler();
            std::string serialize() const;
            void build_error_response( HTTPStatusCode code, std::string reason );
            void handle_request( const HTTPRequest& req );
            void produce( char* buff, size_t max_size );
            
    };
}
