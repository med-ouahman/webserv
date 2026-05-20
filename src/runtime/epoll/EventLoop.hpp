#pragma once

#include <vector>
#include "ListeningSocket.hpp"
#include "Config.hpp"
#include "Result.hpp"

namespace net {
	class Connection;
	class ListeningSocket;
}

namespace cgi {
	class CGIProcess;
}


namespace runtime {
	
	namespace epoll {

	class EventLoop {
		private:
			int epoll_fd;
		
			EventLoop( const EventLoop& other );
			EventLoop& operator=( const EventLoop& other );
			
			void update_epoll_interest( net::Connection* conn ); // questionable?????

		public:
			explicit EventLoop();
			~EventLoop();
			int run(); 
			bool add_connection( int client_fd );
		
			bool register_handler( io::AEventHandler* handler ) const;
			bool modify_handler( io::AEventHandler* handler ) const;
			bool del_handler( io::AEventHandler* handler ) const;
		};
	}
}
