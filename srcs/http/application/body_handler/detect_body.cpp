#include "HTTPDispatcher.hpp"
#include "HTTPRequest.hpp"

namespace http {

	BodyType::Type HTTPDispatcher::detect_body_type( const HTTPRequest& request )  {

		if (request.data().method == GET or request.data().method == DELETE) {
			return BodyType::NONE;
		}

		const std::string& transfer_encoding = request.data().headers.at("transfer-encoding");

		bool has_content_length = not request.data().headers.at("content-length").empty();
		
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
