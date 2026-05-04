
#pragma once

#include <iostream>
#include "IIOHandler.hpp"
#include <unistd.h>
#include "Result.hpp"
#include "IDataListener.hpp"
#include "DataView.hpp"

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
			virtual bool process() = 0;
			virtual bool readbuf_drained() = 0;
			virtual void handle_event() = 0;
			bool processing;
			EventType io_event;
			ssize_t bytes_r; /* whatever read/write returned */

		protected:
			/* input */
			char readbuf[READ_BUFFER_SIZE];
			/* output */
			char writebuff[SEND_CHUNK_SIZE];
			size_t bytes_to_write;
			size_t bytes_sent;
			size_t sent_offset;

			core::DataView data_view;


		protected:
			void read();
			void write();
			void on_readable();
			void on_writeable();
			
			virtual void on_read_eof() = 0;
			virtual void process_incoming_data() = 0;
			virtual void process_outgoing_data() = 0;
			virtual void on_write_complete() = 0;
			virtual void on_write_error() = 0;

	};
}
