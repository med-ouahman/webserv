#include "HTTPDispatcher.hpp"

namespace http {

	BodyType::Type HTTPDispatcher::detect_body_type( std::map<std::string, std::string>& headers )  {

		std::string& transfer_encoding = headers["transfer-encoding"];

		bool has_content_length = not headers["content-length"].empty();
		bool has_transfer_encoding = not transfer_encoding.empty();

		if (has_transfer_encoding and transfer_encoding != "chunked") {
			build_error_response(NOT_IMPLEMENTED, "Not Implemented");
			return BodyType::ERROR;	
		}

		if (has_content_length and has_transfer_encoding)
			return BodyType::ERROR;

		if (has_content_length)	return BodyType::CONTENT_LENGTH;
		
		else if (has_transfer_encoding) return BodyType::TRANSFER_ENCODING_CHUNKED;
		
		return BodyType::NONE;
	}
}
