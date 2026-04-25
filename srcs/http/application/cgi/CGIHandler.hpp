#pragma once

#include "IOHandler.hpp"
#include <unistd.h>

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}

namespace http {
	
	class CGIHandler: public io::IOHandler {
		
		public:
			explicit CGIHandler( const io::EventLoop& loop, const core::Connection& con );
			~CGIHandler();
			void on_event( io::EventType event );

		private:
			enum CGIState {
				SPAWN,
				ACTIVE,
				IDLE,
			} cgi_state;

		private:
			pid_t cgi_pid;
			int cgi_exit_code;
			int pipefds[2];
			const io::EventLoop& loop;
			const core::Connection& conn;

		};

}
