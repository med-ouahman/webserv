#pragma once
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "IOHandler.hpp"
#ifndef NDEBUG
#define NDEBUG 4
#endif
#include <cassert>
#define BACKLOG 5

namespace io {

	class ListeningSocket: public IOHandler {
		private:
			int server_fd;
			EventLoop& loop;
			ListeningSocket( const ListeningSocket& socket );
			ListeningSocket& operator=( const ListeningSocket& socket );
			bool accept_clients();
			bool on_error();
		public:
			int getFd() { return server_fd; }
			ListeningSocket( EventLoop& loop );
			void on_event( uint32_t events );
	};
}
