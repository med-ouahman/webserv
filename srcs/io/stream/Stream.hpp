
#pragma once

#include <iostream>
#include "IIOHandler.hpp"
#include <unistd.h>
#include "Result.hpp"
#include "IDataListener.hpp"
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

	class Stream: public IIOHandler {

		public:
			const static std::size_t READ_BUFFER_SIZE = 1024 * 4;
			const static std::size_t SEND_CHUNK_SIZE = 1024 * 4;

		public:
			Stream( int fd );
			virtual ~Stream() {};
			void on_event( EventType type );
		
		protected:
			virtual void process() = 0;
			virtual void handle_event() = 0;
			bool processing;
			EventType io_event;
			ssize_t bytes_r;

		private:
			/* input */
			char readbuf[READ_BUFFER_SIZE];
			/* output */
			char writebuff[SEND_CHUNK_SIZE];
			
		protected:
			core::DataView data_view;
			core::BufferWriter writer;

		protected:
			void read();
			void write();
			void on_readable();
			void on_writeable();
			
			virtual void on_read_eof() = 0;
			virtual void process_incoming_data() = 0;
			virtual void on_read_error() = 0;
			virtual void process_outgoing_data() = 0;
			virtual void on_write_complete() = 0;
			virtual void on_write_error() = 0;

	};
}
