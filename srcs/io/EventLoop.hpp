#pragma once

namespace io {
	class EventLoop {
		private:
			int epollFd; /* returned by epoll_create1() */
		public:
			EventLoop();
			~EventLoop();
			void run(); /* run the event loop */
	};
}
