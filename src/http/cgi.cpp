
#include "Context.hpp"
#include "CgiHandler.hpp"
#include "FileBodyProvider.hpp"
#include "MemoryBodyProvider.hpp"
#include <cstdlib>
#include <fcntl.h>

namespace http {
 
void Context::on_cgi_ready(const CGIResult result) {

	response.status = result.status_code;

	std::cout << "Response ready\n";
	if (response.status != http::OK) {
		std::cout << "Not okay sorry\n";
		response.headers.add("Connection", "close");
		action_ = AC_CLOSE;
		return;
	}

	response.headers = result.headers;

	if (result.mem_) {
		response.body = new body::MemoryBodyProvider(result.body_);
	} else {
		response.body = new body::FileBodyProvider(result.body_filename);
	}
	
	state_ = WRITING_RESPONSE;
	action_ = AC_WRITE;
}


base::io::Reader& Context::request_body() {
	return request.body;
}


size_t Context::serialize(char* p, size_t size) {
	size_t r = 0;
	std::string headers = "Connection: close\r\nServer: webserv\r\n\r\n";
	::memcpy(p, headers.c_str(), headers.size());
	p += headers.size();
	r+=headers.size();
	size -= headers.size();
	
	if (response.body) {
		r+=response.body->read(p, size);
		std::cout << "r: " << r <<  "\n";
	}

	action_ = AC_READ;
	return r;

}

}

