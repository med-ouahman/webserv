
#include "BodyProvider.hpp"

#include <iostream>

namespace http {


	ssize_t BodyProvider::write_body_content_length( BufferWriter& writer ) {

		size_t w = writer.write(source.data(), source.size());

		body_bytes_sent += w;

		source.advance(w);
		
		if (body_bytes_sent > body_content_length ||
			(body_bytes_sent < body_content_length && w == 0)) return -1;

		
		return body_bytes_sent;
	}

	ssize_t BodyProvider::write_body_chunked( BufferWriter& writer ) {

		std::cout << "current chunk size: " << source.size() << "\n";

		switch (chunk_state) {
			case CHUNK_HEAD: {

				format_chunk(source.remaining());
				
				if (writer.bytes_free() < chunk_header.size()) break;
				
				writer.write(chunk_header.c_str(), chunk_header.size());

				chunk_header.clear();
				
				chunk_state = CHUNK_DATA;
			}

			/* fall through */
		
			case CHUNK_DATA: {

				size_t w = writer.write(source.read_ptr(), source.remaining());
				source.advance(w);

				if (not source.empty()) break;

				chunk_state = CHUNK_TRAIL;
			}

			/* fall through */

			case CHUNK_TRAIL: {

				std::cout << "sending chunk trail\n";
				
				if (writer.bytes_free() < 2) break;
				
				writer.write("\r\n", 2);

				chunk_state = DONE;
				break;
			}

			case DONE:
				return 0;
		}

		return writer.size();
	}
}
	