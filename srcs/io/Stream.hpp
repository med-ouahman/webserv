
#pragma once

#include <iostream>
#include "IIOHandler.hpp"
#include <stddef.h>

namespace io {

	class IOState {
		public:
			enum Type {
				READING,
				WRITING,
				ERROR,
			};
	};

	class Stream: public IIOHandler {

		public:
			const static std::size_t READ_BUFFER_SIZE = 1024 * 16;
			const static std::size_t SEND_CHUNK_SIZE = 1024 * 16;

		public:
			Stream();
			virtual ~Stream() {};
			void on_event( EventType type );
		
		protected:
			virtual void process( void ) = 0;
			virtual void error( void ) = 0;
			IOState::Type io_state;
			
		protected:
			/* input */
			char readbuf[READ_BUFFER_SIZE];
			::ssize_t bytes_received;
			/* output */
			char writebuff[SEND_CHUNK_SIZE];
			::ssize_t bytes_in;
			::ssize_t sent_bytes;
			::size_t sent_offset;

		private:
			void read( void );
			void write( void );
	};
}
