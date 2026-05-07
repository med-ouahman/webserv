#include "HTTPResponse.hpp"
#include <cstring>
#include <iostream>
#include "BufferWriter.hpp"

namespace http {

	ssize_t HTTPResponse::produce( core::BufferWriter* writer, size_t size ) {
		
		ssize_t bytes = 0;
		
		if (HEADERS == serialize_state || RESPONSE_LINE == serialize_state) {
			bytes += serialize_headers(writer->buff(), size);
			return bytes;
		}

		if (BODY == serialize_state) {
			ssize_t b = body_provider->read(writer, size - bytes);
			if (b < 0)
				return -1;
			bytes += b;
		}

		return bytes;
	}
}
