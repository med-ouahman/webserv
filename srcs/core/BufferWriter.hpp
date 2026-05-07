#pragma once

#include <iostream>

namespace core {

	class BufferWriter {

		private:
			char* buff_;
			size_t size_;
			size_t offset_;
		
		public:
			explicit BufferWriter( char* b );
			~BufferWriter();
			bool update( char* buff, size_t size ); /* update the buffer, only possible if the current is empty */
			char* buff();
			size_t offset();
			size_t size();
			bool full();
			size_t remaining();
			void advance( size_t n );
	};

} 
