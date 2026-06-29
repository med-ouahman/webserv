
#include "Context.hpp"
#include "CgiHandler.hpp"
#include "CGIBodyProvider.hpp"
#include <cstdlib>
#include <fcntl.h>

namespace http {
 
void Context::on_cgi_headers(const CgiResult result) {

	response.status = result.status;

	if (response.status != http::OK) {
		response.headers.add("Connection", "close");
		action_ = AC_CLOSE;
		return;
	}

	response.headers = result.headers_;
	
	action_ = AC_WRITE;
}


void Context::on_cgi_error(StatusCode code_) {
	response.status = code_;
	action_ = AC_CLOSE;
}

size_t Context::on_cgi_body(const BufferView& view) {

	response.b.clear();
	response.b.append(view.data(), view.size());
	action_ = AC_WRITE;
	return view.size();
}

base::io::Reader& Context::request_body() {
	return request.body;
}

}

