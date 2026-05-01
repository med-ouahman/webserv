#pragma once

#include "IIOHandler.hpp"
#include "IOChannel.hpp"
#include <unistd.h>
#include <stdlib.h>
#include "PipeGuard.hpp"
#include "IDataListener.hpp"

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

		private:
			enum CGIState {
				SPAWN,
				ACTIVE,
				IDLE,
				ERROR,
			}	cgi_state;

			enum CGIParseState {
				STATUS_LINE,
				HEADERS,
				BODY,
			} output_state;

			

			pid_t	cgi_pid;
			int		cgi_status;

			PipeGuard pipe_guard;
			IOChannel stdin_ch;
			IOChannel stdout_ch;
			IOChannel stderr_ch;
	
			core::Connection& conn;
			const io::EventLoop& loop;
			LineScanner scanner;
		
		public:
			void on_input_ready( char* buff, size_t size );
			void produce_output( char* buff, size_t size );
			size_t consumed_data() { return scanner.get_bytes_consumed(); };
			void on_error();
			/**/

	};

}
