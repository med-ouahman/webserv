#include "HTTPResponse.hpp"

namespace http {

    HTTPResponse::HTTPResponse()
    : status_code(OK),
    body_provider(NULL),
    serialize_state(RESPONSE_LINE) {}

    HTTPResponse::~HTTPResponse() {
        delete body_provider;
    }
	
}
