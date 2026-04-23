
#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"
#include "Config.hpp"
#include <iostream>

namespace http {

    HTTPResponseHandler::HTTPResponseHandler( const config::Config& conf )
        :allow_keep_alive(true),
        config(conf),
        serialize_state(DONE)
         {}

    HTTPResponseHandler::~HTTPResponseHandler() {}

    void HTTPResponseHandler::build_error_response( HTTPStatusCode code, std::string reason ) {
        std::cout << "code: " << code << " reason: " << reason << "\n";
        response.status_code = code;
        response.reason = reason;
        response.headers["Connection"] = "close";
    }

}
