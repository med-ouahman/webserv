
#include "BodyHandler.hpp"

namespace http {

	size_t BodyHandler::parse_chunk_size( const std::string& line_buff ) {
		
		size_t chunk_size = 0;
		
		for ( size_t i(0); i < line_buff.size(); ++i ) {

			char c = ::tolower(line_buff[i]);

			if (c == ';') {
				break;
			}

			if (!is_valid_hexa(c)) {
				return MAX_CHUNK_SIZE + 1;
			}

			chunk_size = chunk_size * hexas.size() + hexas.find(c);

			if (chunk_size > MAX_CHUNK_SIZE) {
				/* I will be generous and log errors later */
				return MAX_CHUNK_EXC;
			}
		}

		return chunk_size;
	}
}
