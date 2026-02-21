#pragma once

#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <stdexcept>
#include "ListeningSocket.hpp"
#include "Config.hpp"

#define MAX_EVENTS 128 // you choose

namespace core {
	class Connection;
}

namespace io {
	class IOHandler;

	class EventLoop {
		private:
			int epollFd;
			bool running;
			struct epoll_event events[MAX_EVENTS];
			std::vector<core::Connection*> conns;
			const config::Config& conf;
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
			void read_from_socket( core::Connection& conn );
            void write_to_socket( core::Connection& conn );
		public:
			bool add_connection( int client_fd );
			bool remove_connection( core::Connection* conn );
			void apply_connection_actions( core::Connection* conn );
			bool add_fd( int fd, uint32_t events, IOHandler* handler );
			bool mod_fd( int fd, uint32_t events, IOHandler* handler );
			bool del_fd( int fd );
			EventLoop( const config::Config& conf );
			~EventLoop();
			void run( ListeningSocket& server ); 
	};
}
