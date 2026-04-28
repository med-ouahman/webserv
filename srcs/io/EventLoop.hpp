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

namespace core {
	class Connection;
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
			const config::Config& conf;
			std::vector<ListeningSocket> listeners;
			std::vector<io::IIOHandler*> pending_handlers;
			std::vector<http::CGIHandler*> cgi_handlers;

		private:
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
		
		private:
			void read_from_socket( core::Connection& conn );
            void write_to_socket( core::Connection& conn );
			error::Result<int> create_listening_socket( const config::ListenEndPoint& endpoint );
			bool start_listeners( void );

		private:
			bool remove_connections( void );
			bool apply_connection_actions( core::Connection* conn );
			void update_epoll_interest( core::Connection* conn );
			void sweep( void );
			
		public:
			void register_io_handler( IIOHandler* io_handler );
			void register_cgi_handler( http::CGIHandler* cgi_handler );
			explicit EventLoop( const config::Config& conf );
			~EventLoop();
			int run( void ); 

		private:
			bool add_fd( int fd, uint32_t events, IIOHandler* handler );
			bool add_connection( int client_fd );
			bool mod_fd( int fd, uint32_t events, IIOHandler* handler );
			bool del_fd( int fd );
		
	};
}
