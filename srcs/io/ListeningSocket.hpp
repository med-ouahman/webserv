#pragma once
#include <iostream>
#include "IIOHandler.hpp"

#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace io {
	class EventLoop;

	class ListeningSocket: public IIOHandler {
		private:
			EventLoop& loop;
			ListeningSocket& operator=( const ListeningSocket& socket );
			bool accept_clients();
			bool on_error();
			
		public:
			ListeningSocket( const ListeningSocket& socket );
			int get_fd() { return fd; }
			explicit ListeningSocket( EventLoop& loop, int fd );
			~ListeningSocket();
			void on_event( EventType event );
			void release();
	};
}
