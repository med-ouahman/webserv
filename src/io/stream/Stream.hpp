
#pragma once

#include <iostream>
#include "AEventHandler.hpp"
#include <unistd.h>
#include "Result.hpp"
#include "DataView.hpp"
#include "BufferWriter.hpp"

namespace io {

	class IStreamDelegate {

		public:
			virtual void consume( DataView& view ) = 0;
			virtual void produce( BufferWriter& w ) = 0;
			virtual void on_stream_error() = 0;
			virtual void on_stream_closed() = 0;

			virtual ~IStreamDelegate() {};
	};

	class Stream: public AEventHandler {

		public:
			const static std::size_t READ_BUFFER_SIZE = 1024 * 4;
			const static std::size_t WRITE_BUFFER_SIZE = 1024 * 4;
			Stream( int fd, EventMask mask );
			virtual ~Stream() {};
			void on_event( EventType type );
		private:

			IStreamDelegate* delegate;
			
			char readbuf[READ_BUFFER_SIZE];
			
			BufferWriter writer;

			void on_readable();
			void on_writeable();
	};
}
