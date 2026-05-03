#include "BodyHandler.hpp"
#include <iostream>

namespace http {

	void BodyHandler::detect_body_type( std::map<std::string, std::string> & headers )  {
		
		if (body_set)
			return ;
		
		bool content_length = headers["content-length"].size() != 0;
		bool transfer_encoding = headers["transfer-encoding"].size() != 0;

		if (content_length && transfer_encoding)
			body_type = BodyType::ERROR;

		if (content_length) {
			std::cout << "content-length: what the hell??";
			if (!parse_content_length(headers["content-length"]))
				body_type = BodyType::ERROR;

			if (body_len == 0) body_type = BodyType::NONE;
			else body_type = BodyType::CONTENT_LENGTH;
			
		} 
		
		else if (transfer_encoding) body_type = BodyType::TRANSFER_ENCODING_CHUNKED;
		
		else body_type = BodyType::NONE;
		
	}
}
