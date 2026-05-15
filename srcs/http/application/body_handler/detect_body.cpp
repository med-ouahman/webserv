#include "HTTPDispatcher.hpp"
#include "HTTPRequest.hpp"

namespace http {

	BodyType::Type HTTPDispatcher::detect_body_type( const HTTPRequest& request )  {

		if (request.method == GET or request.method == DELETE) {
			return BodyType::NONE;
		}

		std::string& transfer_encoding = const_cast<std::map<std::string, std::string>&>(request.headers)["transfer-encoding"];

		bool has_content_length = not const_cast<std::map<std::string, std::string>&>(request.headers)["content-length"].empty();
		
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
