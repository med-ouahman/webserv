
#include "HTTPRequest.hpp"
#include "HTTPDispatcher.hpp"
#include "Config.hpp"
#include <iostream>

namespace http {
    const char* HTTPDispatcher::CRLF = "\r\n";
    const char* HTTPDispatcher::COLON = ": ";
    
    HTTPDispatcher::HTTPDispatcher( const config::Config& conf )
        :allow_keep_alive(true),
        config(conf),
        serialize_state(DONE)
         {}

    HTTPDispatcher::~HTTPDispatcher() {}

    void HTTPDispatcher::build_error_response( HTTPStatusCode code, std::string reason ) {
        std::cout << "code: " << code << " reason: " << reason << "\n";
        response.status_code = code;
        response.reason = reason;
        response.headers["Connection"] = "close";
        response.body = "HTTP/1.1 403 Forbidden\r\nConnection: close\r\n";
    }

}
