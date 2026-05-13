#pragma once

#include <stdio.h>
#include <vector>
#include <stdexcept>
#include "ListeningSocket.hpp"
#include "Config.hpp"
#include "Result.hpp"
#include <ctime>
#include <chrono>
#include <list>

namespace core {
	class Connection;
}

namespace http {
	class CGIHandler;
}

namespace io {
	class IIOHandler;

	typedef int second_t;

	class EventLoop {
		private:
			const static std::size_t	MAX_CONNECTIONS		= 1000;
			const static std::size_t	MAX_EVENTS			= 128;
			const static std::size_t	MAX_CONCURRENT_CGI	= 100;
			const static std::size_t	MAX_TIMEOUT_MS		= 60;

			const static second_t		MAX_HEADER_TIMEOUT	= 10;
			const static second_t		MAX_BODY_PROGRESS_TIMEOUT = 30;
			const static second_t		MAX_IDLE_TIMEOUT = 60;

		private:
			int epoll_fd;
			bool running;
			std::vector<core::Connection*> conns;
			std::vector<ListeningSocket> listeners;
			std::vector<http::CGIHandler*> cgi_bin;

		private:
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
			error::Result<int> create_listening_socket( const config::ListenEndPoint& endpoint );
			bool start_listeners();
			void sweep();
			void update_epoll_interest( core::Connection* conn );
			bool timedout( core::Connection* conn );
			
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
