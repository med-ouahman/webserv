#include "HTTPResponse.hpp"
#include <cstring>
#include <iostream>
#include "BufferWriter.hpp"

namespace http {


	void HTTPResponse::build_error_response( HTTPStatusCode code ) {
        std::cout << "Sure shit, code: " << code << "\n";
        status_code = code;
        headers["Connection"] = "close";
    }


	ssize_t HTTPResponse::produce( BufferWriter* writer ) {
		
		ssize_t bytes = 0;
		
		std::cout << "Begin serializing the response\n";
		
		if (HEADERS == serialize_state || RESPONSE_LINE == serialize_state) {
			bytes += serialize_headers(writer);
			std::cout << "Bytes Serialized: " << bytes << "\n";
		}

		if (BODY == serialize_state && body_provider not_eq NULL) {
			std::cout << "Begin serializing the body\n";
			
			ssize_t b = body_provider->read(writer);
			if (b < 0)
				return -1;
			bytes += b;
		}
		
		if (bytes == 0) std::cout << "Response done\n";
		return bytes;
	}

}
