#pragma once

#include "limits.hpp"
#include <vector>
#include "ListeningSocket.hpp"
#include "Config.hpp"
#include "Result.hpp"
#include <ctime>

namespace core {
	class Connection;
}

namespace http {
	class CGIRequestHandler;
}

namespace io {
	class AEventHandler;

	typedef int second_t;

	class EventLoop {
		private:
			int epoll_fd;
			bool running;
			std::vector<core::Connection*> conns;
			std::vector<ListeningSocket> listeners;
			std::vector<http::CGIRequestHandler*> cgi_bin;
			
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
			error::Result<int> create_listening_socket( const config::ListenEndPoint& endpoint );
			bool start_listeners();
			void sweep();
			void update_epoll_interest( core::Connection* conn );
		public:
			explicit EventLoop();
			~EventLoop();
			int run(); 
			bool add_connection( int client_fd );
			
			bool add_fd( int fd, uint32_t events, AEventHandler* handler ) const;
			bool mod_fd( int fd, uint32_t events, AEventHandler* handler ) const;
			bool del_fd( int fd ) const;
			void add_cgi_handler( http::CGIRequestHandler* h );
		
	};
}
