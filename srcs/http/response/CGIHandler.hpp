#pragma once

#include "IOHandler.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"

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
	};

}
