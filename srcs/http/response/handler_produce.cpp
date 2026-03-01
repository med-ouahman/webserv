#include "HTTPResponseHandler.hpp"

namespace http {

	void HTTPResponseHandler::produce( char* buff, size_t max_size ) {
		serialize_state = DONE;
	}
}
