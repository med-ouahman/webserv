#pragma once

#include "IOHandler.hpp"

namespace io {
	class EventLoop;
}

namespace core {
	class Connection;
}

namespace http {
	
	class CGIHandler: public io::IOHandler {
		
		public:
			void on_event( io::EventType event );
			explicit CGIHandler( const io::EventLoop& loop, const core::Connection& con );
			~CGIHandler();

		private:
			enum CGIState {
				ACTIVE,
				IDLE,
			} cgi_state;
			
		private:
			const io::EventLoop& loop;
			const core::Connection& conn;
			// const config::Config& config;
	};

}
