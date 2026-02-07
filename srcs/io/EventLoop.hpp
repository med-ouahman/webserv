#pragma once

#include <sys/epoll.h>
#include <vector>
#include <unistd.h> 
#include <stdexcept>
#define NDEBUG 0
#include <cassert>

#define MAX_EVENTS 64

namespace core {
	class Connection; /* forward declaration */
}

namespace io {
	class EventLoop {
		private:
			int epollFd;
			std::vector<struct epoll_event> events;
				/* managed connections, 
					connections are contained internally via void* data pointer in epoll_event
				*/
			EventLoop(const EventLoop& other); /* non-copyable */
			EventLoop& operator=(const EventLoop& other); /* non-assignable */
		public:
			void add_connection(core::Connection* conn);
			void remove_connection(core::Connection* conn);
			void apply_connection_actions(core::Connection* conn);
			EventLoop();
			~EventLoop();
			void run(); /* run the event loop */
	};
}
