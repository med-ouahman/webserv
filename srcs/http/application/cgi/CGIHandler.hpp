#pragma once

#include "IIOHandler.hpp"
#include "IOChannel.hpp"
#include <unistd.h>
#include <stdlib.h>
#include "PipeGuard.hpp"
#include "IDataListener.hpp"
#include "LineScanner.hpp"

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}


namespace http {
	
	struct CGIContext;

	class CGIHandler: public io::IDataListener {
		
		public:
			explicit CGIHandler( core::Connection& conn, const io::EventLoop& l );
			~CGIHandler();
			void spawn( const CGIContext& context );
			const static std::size_t MAX_BLOCK_LEN = 1024 * 16;
			
		private:
			enum CGIState {
				SPAWN,
				ACTIVE,
				IDLE,
				ERROR,
			}	cgi_state;

			enum CGIOutputState {
				STATUS_LINE,
				HEADERS,
				BODY,
			} output_state;

			enum CGIError {
				NONE,
				TIMEOUT,
				CRASH,
				PIPE_FAIL,
			} cgi_error;

			pid_t	cgi_pid;
			int		cgi_status;

			PipeGuard pipe_guard;
			IOChannel stdin_ch;
			IOChannel stdout_ch;
			IOChannel stderr_ch;
	
			core::Connection& conn;
			const io::EventLoop& loop;
			LineScanner scanner;

			size_t bytes_consumed;
		
		public:
			void on_channel_closed();
			ScanResult on_input_ready( core::DataView* data_view );
			ssize_t produce_output( char* buff, size_t size );
			size_t consumed_data() { return bytes_consumed; };
			void on_error();
		
			/**/

	};

}
