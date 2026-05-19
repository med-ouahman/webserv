#include "Dispatcher.hpp"
#include "Request.hpp"

namespace http {

	BodyType::Type Dispatcher::detect_body_type( const Request& request )  {

		if (request.data().method == GET or request.data().method == DELETE) {
			return BodyType::NONE;
		}

		const std::string& transfer_encoding = request.data().headers.get("transfer-encoding");

		bool has_content_length = not request.data().headers.get("content-length").empty();
		
		bool has_transfer_encoding = not transfer_encoding.empty();

		if (has_transfer_encoding and transfer_encoding != "chunked") {
			// build_error_response(NOT_IMPLEMENTED);
			return BodyType::ERROR;	
		}

		if (has_content_length and has_transfer_encoding)
			return BodyType::ERROR;

		if (has_content_length)	return BodyType::CONTENT_LENGTH;
		
		else if (has_transfer_encoding) return BodyType::TRANSFER_ENCODING_CHUNKED;
		
		return BodyType::NONE;
	}
}
