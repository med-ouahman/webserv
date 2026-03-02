#pragma once

#include <unistd.h>
#include <sys/epoll.h>
#include <vector>
#include <stdexcept>
#include "ListeningSocket.hpp"
#include "Config.hpp"

namespace core {
	class Connection;
}

namespace io {
	class IOHandler;

	class EventLoop {
		private:
			const static size_t READ_BUFFER_SIZE = 1024 * 16;
			const static size_t MAX_EVENTS = 128;

		private:
			int epoll_fd;
			bool running;
			std::vector<core::Connection*> conns;
			const config::Config& conf;
			std::vector<ListeningSocket> listeners;

		private:
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
		
		private:
			void read_from_socket( core::Connection& conn );
            void write_to_socket( core::Connection& conn );
			static int create_listening_socket( const config::ListenEndPoint& endpoint );
			bool start_listeners( void );

		private:
			bool remove_connections( void );
			void apply_connection_actions( core::Connection* conn );
			void update_epoll_interest( core::Connection* conn );
			
		public:
			bool add_fd( int fd, uint32_t events, IOHandler* handler );
			bool add_connection( int client_fd );
			bool mod_fd( int fd, uint32_t events, IOHandler* handler );
			bool del_fd( int fd );
			explicit EventLoop( const config::Config& conf );
			~EventLoop();
			void run( void ); 

	};
}
