#include "HTTPResponseHandler.hpp"
#include <cstring>
#include <iostream>

namespace http {

	::ssize_t HTTPResponseHandler::produce( char* buff, size_t max_size ) {
		/* this fills buff with response left data and goes on until sate == DONE */
		/* max_size is the maximun number of bytes that can be put in buff */
		size_t bytes = std::min(response.body.size(), max_size);
		std::memcpy(buff, response.body.c_str(), bytes);
		response.body.clear();
		return bytes;
	}
}
