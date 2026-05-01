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
			static const int 	  MAX_TIMEOUT_MS		= 60;
			
		private:
			int epoll_fd;
			bool running;
			std::vector<core::Connection*> conns;
			std::vector<ListeningSocket> listeners;

		private:
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
		
		private:
			error::Result<int> create_listening_socket( const config::ListenEndPoint& endpoint );
			bool start_listeners( void );

		private:
			bool remove_connections( void );
			void update_epoll_interest( core::Connection* conn );
			
		public:
			explicit EventLoop( const config::Config& conf );
			~EventLoop();
			int run( void ); 
			const config::Config& conf;
			bool add_connection( int client_fd );

			bool add_fd( int fd, uint32_t events, IIOHandler* handler ) const;
			bool mod_fd( int fd, uint32_t events, IIOHandler* handler ) const;
			bool del_fd( int fd ) const;
		
	};
}
