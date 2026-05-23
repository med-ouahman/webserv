#include "Response.hpp"
#include <cstring>
#include <iostream>
#include "BufferWriter.hpp"

namespace http {



	ssize_t Response::produce( BufferWriter& writer ) {
		
		ssize_t bytes = 0;
		
		std::cout << "Begin serializing the response\n";
		
		if (HEADERS == state || RESPONSE_LINE == state) {
			bytes += serialize(writer);
			std::cout << "Bytes Serialized: " << bytes << "\n";
		}

		if (BODY == state && body_provider) {
			std::cout << "Begin serializing the body\n";
			
			ssize_t b = body_provider->read(writer);
			
			if (b < 0) return -1;
			
			bytes += b;
		}
		
		if (bytes == 0) {
			std::cout << "Response done\n";
			state = DONE;
		}

		return bytes;
	}

}
