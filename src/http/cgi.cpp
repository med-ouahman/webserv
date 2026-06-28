
#include "Context.hpp"
#include "CgiHandler.hpp"
#include "CGIBodyProvider.hpp"
#include <cstdlib>
#include <fcntl.h>

namespace http {
 
void Context::on_cgi_ready(const CgiResult result) {

	response.status = result.code_;

	if (response.status != http::OK) {
		response.headers.add("Connection", "close");
		action_ = AC_CLOSE;
		return;
	}

	response.headers = result.headers_;

	CgiHandler& h = static_cast<CgiHandler&>(*handler);

	response.body = new CGIBodyProvider(h, result.body_source);
	
	state_ = WRITING_RESPONSE;
	action_ = AC_WRITE;
}


void Context::on_cgi_error(StatusCode code_) {
	response.status = code_;
	action_ = AC_CLOSE;
}

base::io::Reader& Context::request_body() {
	return request.body;
}

}

