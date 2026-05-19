
#pragma once

#include <iostream>
#include "AEventHandler.hpp"
#include <unistd.h>
#include "Result.hpp"
#include "DataView.hpp"
#include "BufferWriter.hpp"

namespace io {

	class IOState {
		public:
			enum Type {
				READING,
				WRITING,
				ERROR,
			};
	};

	class Stream: public AEventHandler {

		public:
			const static std::size_t READ_BUFFER_SIZE = 1024 * 4;
			const static std::size_t WRITE_BUFFER_SIZE = 1024 * 4;
			Stream( int fd );
			virtual ~Stream() {};
			void on_event( EventType type );
		private:
			char readbuf[READ_BUFFER_SIZE];
			char writebuff[WRITE_BUFFER_SIZE];
			
		protected:
			EventType io_event;
			ssize_t bytes_r;
			DataView data_view;
			BufferWriter writer;
			void read();
			void write();
			void on_readable();
			void on_writeable();
			void on_error();

	};
}
