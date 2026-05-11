#pragma once

#include <stdio.h>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <stdexcept>
#include "ListeningSocket.hpp"
#include "Config.hpp"
#include "Result.hpp"
#include <fcntl.h>
#include <deque>
#include <ctime>

namespace core {
	class Connection;
}

namespace http {
	class CGIHandler;
}

namespace io {
	class IIOHandler;

	class EventLoop {
		private:
			const static ::size_t MAX_EVENTS			= 128;
			const static ::size_t MAX_CONCURRENT_CGI	= 100;
			static const int 	  MAX_TIMEOUT_MS		= 60; // static for now, soon will bey dynamic using max/min heap
			
		private:
			int epoll_fd;
			bool running;
			std::vector<core::Connection*> conns;
			std::vector<ListeningSocket> listeners;
			std::deque<core::Connection*> pending_conns;
			std::vector<http::CGIHandler*> bin;

		private:
			EventLoop( const EventLoop& other ); // Non copy-able
			EventLoop& operator=( const EventLoop& other ); // Non assignable
			error::Result<int> create_listening_socket( const config::ListenEndPoint& endpoint );
			bool start_listeners();
			void sweep();
			void update_epoll_interest( core::Connection* conn );

		public:
			explicit EventLoop();
			~EventLoop();
			int run(); 
			bool add_connection( int client_fd );
			
			bool add_fd( int fd, uint32_t events, IIOHandler* handler ) const;
			bool mod_fd( int fd, uint32_t events, IIOHandler* handler ) const;
			bool del_fd( int fd ) const;
			void add_cgi_handler( http::CGIHandler* h );
		
	};
}
