#include "HTTPDispatcher.hpp"
#include <iostream>
#include <cstdlib>

namespace http {

	BodyType::Type HTTPDispatcher::detect_body_type( std::map<std::string, std::string>& headers )  {

		bool content_length = headers["content-length"].size() != 0;
		bool transfer_encoding = headers["transfer-encoding"].size() != 0;

		if (content_length && transfer_encoding)
			return BodyType::ERROR;

		if (content_length)	return BodyType::CONTENT_LENGTH;
			
		else if (transfer_encoding) return BodyType::TRANSFER_ENCODING_CHUNKED;
		
		return BodyType::NONE;
	}
}
