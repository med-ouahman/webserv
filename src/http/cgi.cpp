#include "Context.hpp"
#include "CgiHandler.hpp"
#include "FileBodyProvider.hpp"
#include <cstdlib>

namespace http {
 
void Context::on_cgi_ready() {

	CgiHandler& cgi_handler = *static_cast<CgiHandler*>(handler);

	CGIResult result = cgi_handler.result();

	std::cout << "Satus Code: " << result.status_code << "\n";
	
	response.status = result.status_code;

	if (result.status_code != http::OK) return;

	response.headers = result.headers;

	::lseek(result.body_fd, 0, 0);

	response.body = const_cast<body::FileBodyProvider*>(new body::FileBodyProvider(result.body_fd));
	response.encoder = body::BodyEncoder(base::sizeof_file(result.body_filename.c_str()));
	
	state_ = WRITING_RESPONSE;
	action_ = AC_WRITE;
}


}
