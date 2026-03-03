
#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"
#include "Config.hpp"
#include <iostream>

namespace http {

    HTTPResponseHandler::HTTPResponseHandler(): allow_keep_alive(true) {}

    HTTPResponseHandler::~HTTPResponseHandler() {}

    void HTTPResponseHandler::build_error_response( HTTPStatusCode code, std::string reason ) {
        std::cout << "Error response: code: " << code << " reason: " << reason << "\n";
        response.status_code = code;
        response.reason = reason;
        response.headers["Content-Length"] = "0";
        response.headers["Connection"] = "close";
    }

}
