#include "Context.hpp"
#include "CgiHandler.hpp"
#include "FileBodyProvider.hpp"
#include <cstdlib>
#include <fcntl.h>

namespace http {
 
void Context::on_cgi_ready(const CGIResult result) {

	
	response.status = result.status_code;

	if (result.status_code != http::OK) return;

	response.headers = result.headers;

	response.body = const_cast<body::FileBodyProvider*>(new body::FileBodyProvider(result.body_filename));
	response.encoder = body::BodyEncoder(result.body_content_length);
	
	state_ = WRITING_RESPONSE;
	action_ = AC_WRITE;
}


}
