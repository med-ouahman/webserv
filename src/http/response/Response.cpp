#include "Response.hpp"

namespace http {

    Response::Response()
    : status_code(OK),
    body_provider(NULL),
    state(RESPONSE_LINE) {}

    Response::~Response() {
        delete body_provider;
    }
	
}
