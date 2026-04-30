
#pragma once

#include <iostream>
#include "IIOHandler.hpp"
#include <unistd.h>
#include "Result.hpp"
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
			Stream( int fd );
			virtual ~Stream() {};
			void on_event( EventType type );
		
		protected:
			virtual bool process( void ) = 0;
			virtual void error( void ) = 0;
			virtual bool readbuf_drained() = 0;
			virtual void handle_event( void ) = 0;
			EventType io_event;
			ssize_t bytes_r; /* whatever read/write returned */
			
		protected:
			/* input */
			char readbuf[READ_BUFFER_SIZE];
			size_t bytes_received;
			size_t bytes_consumed;

			/* output */
			char writebuff[SEND_CHUNK_SIZE];
			size_t bytes_to_write;
			size_t bytes_sent;
			size_t sent_offset;

		protected:
			void read( void );
			void write( void );
			virtual void on_readable( void ) = 0;
			virtual void on_writeable( void ) = 0;
	};
}
