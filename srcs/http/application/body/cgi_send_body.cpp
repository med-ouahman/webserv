#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include "BufferWriter.hpp"

namespace http {


	ssize_t CGIBodyProvider::fill_buff( core::BufferWriter* writer ) {

		ssize_t total_copied = 0;

		while (true) {

			cgi_handler.pull();

        	if (cgi_handler.get_cgi_state() == CGIState::WAITING) return writer->size();
			if (cgi_handler.get_cgi_state() == CGIState::ERROR) return -1;

			size_t available = data_view.size() - data_view.cursor();

			size_t to_copy = std::min(available, writer->remaining());
			
			::memcpy(writer->write_ptr(), data_view.data(), to_copy);
			total_copied += to_copy;
			data_view.advance(to_copy);

			writer->advance(to_copy);

			if (writer->full())
				break;
		}

		return writer->size();
	}

	ssize_t CGIBodyProvider::send_body_content_length( core::BufferWriter* writer ) {

		ssize_t t = fill_buff(writer);
		
		if (t < 0) return -1;
		
		body_bytes_read += t;
		
		if (body_bytes_read > content_length || (body_bytes_read < content_length && cgi_handler.finished())) {
			
			cgi_handler.on_error();
			return -1;
		}

		return t;
	}

	ssize_t CGIBodyProvider::send_body_chunked( core::BufferWriter* writer ) {
		std::cout << "Begin sending the body using chunks\n";

		while (true) {

			switch (chunk_state) {
				case ChunkState::CHUNK_HEAD: {
	
					ssize_t t = fill_buff(writer);
					std::cout << "Av: " << writer->size() << "\n";
					if (t < 0) return -1;	
					format_chunk(t);
					chunk_state = ChunkState::CHUNK_DATA;
					temp_writer.update(writer->data(), writer->remaining());
					writer->update(const_cast<char*>(chunk_header.c_str()), chunk_header.size());
					return chunk_header.size();
				}
				

				case ChunkState::CHUNK_DATA:
					writer->update(temp_writer.data(), temp_writer.remaining());
					chunk_state = ChunkState::CHUNK_TRAIL;
					/* fall through */
				case ChunkState::CHUNK_TRAIL:
					if (writer->remaining() >= 2) {
						::memcpy(writer->write_ptr(), "\r\n", 2);
						chunk_state = ChunkState::DONE;
					}
					break;
				case ChunkState::DONE:
					return 0;
				default:
					break;
			}
			
		}
		
	}
}
	