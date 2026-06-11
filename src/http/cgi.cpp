#include "Context.hpp"
#include "CGIRequestHandler.hpp"
#include "CGIBodyProvider.hpp"

namespace http {
 
void Context::on_cgi_ready(const CGIResult& result) {
	response.status = result.code;
	response.headers = result.headers;
	response.body = new CGIBodyProvider(result.source_);
	
	/*
		transition to writing.
	*/

}

}
