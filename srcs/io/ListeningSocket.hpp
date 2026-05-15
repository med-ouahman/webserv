#pragma once
#include <iostream>
#include "AIOHandler.hpp"

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace io {
	class EventLoop;

	class ListeningSocket: public AIOHandler {
		private:
			EventLoop& loop;
			ListeningSocket& operator=( const ListeningSocket& socket );
			bool accept_clients();
			bool on_error();
			
		public:
			ListeningSocket( const ListeningSocket& socket );
			explicit ListeningSocket( EventLoop& loop, int fd );
			~ListeningSocket();
			void on_event( EventType event );
			void release();
	};
}
