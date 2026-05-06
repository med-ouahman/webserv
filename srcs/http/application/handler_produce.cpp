#include "HTTPResponse.hpp"
#include <cstring>
#include <iostream>

namespace http {

	ssize_t HTTPResponse::produce( char* buff, size_t size ) {
		
		ssize_t bytes = 0;
		
		if (HEADERS == serialize_state || RESPONSE_LINE == serialize_state) {
			bytes += serialize_headers(buff, size);
			/* can fall through */
		}

		if (BODY == serialize_state) {
			ssize_t b = body_provider->read(buff, size - bytes);
			if (b < 0)
				return -1;
			bytes += b;
		}

		return bytes;
	}
}
