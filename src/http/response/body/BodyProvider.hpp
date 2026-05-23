#pragma once 

#include "BufferWriter.hpp"
#include <sys/types.h>
#include "DataView.hpp"
#include "Types.hpp"

namespace http {

	

	struct BodyContext {
		
		BodyType	type;
		size_t		content_length;
		DataView&	source;

		BodyContext(BodyType t, size_t len, DataView& v): type(t), content_length(len), source(v) {

		}
	};

	class BodyProvider {

		private:
			DataView&			source;
			const BodyType		type;
			const size_t 		body_content_length;

			ChunkState			chunk_state;
			std::string			chunk_header;

			size_t		body_bytes_sent;

			void	format_chunk( size_t chunk_size );
            ssize_t write_body_content_length( BufferWriter& writer );
            ssize_t write_body_chunked( BufferWriter& writer );

		public:
			BodyProvider( const BodyContext& ctx );
			~BodyProvider();
			ssize_t read( BufferWriter& writer );
	};

}
