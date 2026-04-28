#pragma once

#include "IIOHandler.hpp"
#include "IOChannel.hpp"
#include <unistd.h>
#include <stdlib.h>
#include "PipeGuard.hpp"

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}


namespace http {
	
	struct CGIContext;

	class CGIHandler {
		
		public:
			explicit CGIHandler( const io::EventLoop& loop, const core::Connection& con );
			~CGIHandler();
			void spawn( const CGIContext& context );
			void handle_event( io::EventType event_type, Stream::Type stream );

		private:
			enum CGIState {
				SPAWN,
				ACTIVE,
				ERROR,
				IDLE,
			} cgi_state;

			pid_t	cgi_pid;
			int		cgi_status;

			PipeGuard pipe_guard;
			IOChannel stdin_ch;
			IOChannel stdout_ch;
			IOChannel stderr_ch;
			
			io::EventLoop& loop;
			const 	core::Connection& conn;

	};

}
