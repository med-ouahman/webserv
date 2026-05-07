#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include "BufferWriter.hpp"

namespace http {

	ssize_t CGIBodyProvider::pull() {

		cgi_handler.pull();

        if (cgi_handler.get_cgi_state() == CGIState::ERROR) return -1;
		
		return data_view.size();
	}

	ssize_t CGIBodyProvider::fill_buff( core::BufferWriter* writer ) {

		ssize_t total_copied = 0;

		while (true) {
			
			ssize_t ret = pull();
			
			if (ret < 0) return ret;
			if (ret == 0) return total_copied;

			size_t available = data_view.size() - data_view.cursor();

			size_t to_copy = std::min(available, writer->remaining());
			
			::memcpy(writer->buff(), data_view.data(), to_copy);
			total_copied += to_copy;
			data_view.advance(to_copy);

			writer->advance(to_copy);

			if (writer->full())
				break;
		}

		return total_copied;
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

		while (true) {

			switch (chunk_state) {
				case ChunkState::CHUNK_HEAD:

                	ssize_t t = fill_buff(writer);
					if (t < 0) return -1;	
					format_chunk(t);
					chunk_state = ChunkState::CHUNK_DATA;
					break;

				case ChunkState::CHUNK_DATA:
                /**/
				case ChunkState::CHUNK_TRAIL:
                /**/
				default:
                /**/
			}
			
		}
		
	}
}
	