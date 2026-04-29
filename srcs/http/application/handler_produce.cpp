#include "HTTPDispatcher.hpp"
#include <cstring>
#include <iostream>

namespace http {

	::ssize_t HTTPDispatcher::produce( char* buff, ::size_t size ) {
		
		::memcpy(buff, response.body.c_str(), response.body.size());
		return response.body.size();
		::size_t bytes = 0;

		if (HEADERS == serialize_state || RESPONSE_LINE == serialize_state) {
			bytes += serialize_headers(buff, size);
		}

		if (BODY == serialize_state) {
			::ssize_t b = response.body_provider->read(buff, size - bytes);
			return b;
			
		}

		return 0;
	}
}
