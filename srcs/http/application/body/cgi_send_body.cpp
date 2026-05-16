#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include "BufferWriter.hpp"

namespace http {


	ssize_t CGIBodyProvider::send_body_content_length( BufferWriter* writer ) {

		body_bytes_read += writer->size();
		
		if (body_bytes_read > content_length || (body_bytes_read < content_length && cgi_handler.finished())) {
			
			cgi_handler.on_error();
			return -1;
		}

		return body_bytes_read;
	}

	ssize_t CGIBodyProvider::send_body_chunked( BufferWriter* writer ) {
		std::cout << "Begin sending the body using chunks\n";

		std::cout << "current chunk size: " << data_view.size() << "\n"; 
		switch (chunk_state) {
			case ChunkState::CHUNK_HEAD: {

				chunk_head();
				format_chunk(data_view.size());
				std::cout << chunk_header << "\n";
				chunk_state = ChunkState::CHUNK_DATA;
				temp_writer.update(writer->data(), writer->remaining());
				writer->update(const_cast<char*>(chunk_header.c_str()), chunk_header.size());
				return chunk_header.size();
			}
		
			case ChunkState::CHUNK_DATA:
				chunk_data();
				writer->update(temp_writer.data(), temp_writer.remaining());
				data_view.advance(writer->write(data_view.data(), data_view.size()));
				chunk_state = ChunkState::CHUNK_TRAIL;
				/* fall through */
			case ChunkState::CHUNK_TRAIL:
				std::cout << "sending chunk trail\n";
				chunk_trail();
				if (2 == writer->write("\r\n", 2)) {
					chunk_state = ChunkState::DONE;
				}
				break;
			case ChunkState::DONE:
				return 0;
			default:
				break;
		}

		return 0;
	}
}
	