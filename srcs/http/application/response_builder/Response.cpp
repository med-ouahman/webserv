#include "Response.hpp"

namespace http {

    Response::Response()
    : status_code(OK),
    body_provider(NULL),
    serialize_state(RESPONSE_LINE) {}

    Response::~Response() {
        delete body_provider;
    }
	
}
