#include "HTTPParser.hpp"

namespace http {

	HTTPParser::BodyType::Type HTTPParser::detect_body_type( void )  {
		
		bool content_length = request.headers["content-length"].size() != 0;
		bool transfer_encoding = request.headers["transfer-encoding"].size() != 0;

		if (content_length && transfer_encoding) {
			return BodyType::ERROR;
		}

		if (content_length) {
			if (!parse_content_length(request.headers["content-length"], body_len)) {
				return BodyType::ERROR;
			}

			if (body_len == 0) {
				return BodyType::NONE;
			}

			return BodyType::CONTENT_LENGTH;
		} else {
			
		}
		
		return BodyType::NONE;
	}
}
