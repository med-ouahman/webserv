
#pragma once

#include "HTTPResponse.hpp"

namespace http {

    struct HTTPRequest;

    class HTTPResponseHandler {
        private:
            HTTPResponse response;
        private:
            enum HTTPResponeAction {
                STATIC_FILE,
            };

        public:
            HTTPResponseHandler();
            ~HTTPResponseHandler();
            std::string serialize() const;
            void handle_request( const HTTPRequest& req );
    };
}
