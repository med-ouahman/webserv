#include "HTTPResponse.hpp"
#include <cstring>
#include <iostream>

namespace http {

	::ssize_t HTTPResponse::produce( char* buff, ::size_t size ) {
		
		::memcpy(buff, body.c_str(), body.size());

		::size_t bytes = body.size();
		body.clear();
		return bytes;
		
		if (HEADERS == serialize_state || RESPONSE_LINE == serialize_state) {
			bytes += serialize_headers(buff, size);
		}

		if (BODY == serialize_state) {
			::ssize_t b = body_provider->read(buff, size - bytes);
			return b;
		}

		return 0;
	}
}
