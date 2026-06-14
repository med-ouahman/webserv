#include "Context.hpp"
#include "CgiHandler.hpp"
#include "CGIBodyProvider.hpp"
#include <cstdlib>

namespace http {
 
void Context::on_cgi_ready(const CGIResult result) {

	std::cout << "Satus Code: " << result.status_code << "\n";

	response.status = result.status_code;
	response.headers = result.headers;
	response.body = const_cast<CGIBodyProvider*>(new CGIBodyProvider(result.source_));
	
	const std::string& s = response.headers.get("content-length");

	if (!s.empty())
	{
		char* end = NULL;

		size_t content_length = ::strtoul(s.c_str(), &end, 10);
		if (end && *end != '\0')
		{
			state_ = ERROR;
			action_ = AC_CLOSE;
			return;
		}

		response.encoder = body::BodyEncoder(content_length);
	}
	std::cout << "Ready\n";
	action_ = AC_WRITE;
	
}

}
