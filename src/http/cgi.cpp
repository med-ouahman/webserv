
#include "Context.hpp"
#include "CgiHandler.hpp"
#include "FileBodyProvider.hpp"
#include <cstdlib>
#include <fcntl.h>

namespace http {
 
void Context::on_cgi_ready(const CGIResult result) {

	response.status = result.status_code;

	if (response.status != http::OK) {
		response.headers.add("Connection", "close");
		action_ = AC_CLOSE;
		return;
	}

	response.headers = result.headers;
	
	
	state_ = WRITING_RESPONSE;
	action_ = AC_WRITE;
}


base::io::Reader& Context::request_body() {
	return request.body;
}

}

