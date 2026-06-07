
#include "HTTPResponseHandler.hpp"
#include <sstream>

namespace http {
    void HTTPResponseHandler::serialize( void ) {
        response.body = "HTTP\1.1 ";
        std::stringstream ss;
        ss << response.status_code;
        response.body.append(ss.str());
        response.body.append(" " + response.reason + "\r\n\r\n");
    }
}
