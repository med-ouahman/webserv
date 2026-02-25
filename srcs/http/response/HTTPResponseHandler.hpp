
#pragma once

#include "HTTPResponse.hpp"

namespace config {
    struct ServerConfig;
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

        private:
            ResolutionResult resolve( const HTTPRequest& req, const config::ServerConfig& server ) const;
            static const config::LocationConfig* find_location( const std::string& url, const config::ServerConfig& server );
        public:
            HTTPResponseHandler();
            ~HTTPResponseHandler();
            std::string serialize() const;
            void handle_request( const HTTPRequest& req );
    };
}
