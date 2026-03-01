
#include "HTTPRequest.hpp"
#include "HTTPResponseHandler.hpp"
#include "Config.hpp"


namespace http {

    HTTPResponseHandler::HTTPResponseHandler() {}

    HTTPResponseHandler::~HTTPResponseHandler() {}

    void HTTPResponseHandler::build_error_response( HTTPStatusCode code, std::string reason ) {
        response.status_code = code;
        response.reason = reason;
        response.headers["Content-Length"] = "0";
        response.headers["Connection"] = "close";
    }

}
