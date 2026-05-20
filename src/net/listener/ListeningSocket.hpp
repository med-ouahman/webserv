#pragma once

#include "AEventHandler.hpp"
#include "Result.hpp"

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace net {

	class EventLoop;

	class ListeningSocket: public io::AEventHandler {
		private:
			ListeningSocket& operator=( const ListeningSocket& socket );
			bool accept_clients();
			bool on_error();
			
		public:
			ListeningSocket( const ListeningSocket& socket );
			explicit ListeningSocket( EventLoop& loop, int fd );
			~ListeningSocket();
			void on_event( io::EventType event );
			void release();
	};

	Base::Result<std::vector<ListeningSocket> > create_listening_sockets( const std::vector<config::ListenEndPoint> endpoints );
}
