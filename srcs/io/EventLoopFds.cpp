#include "EventLoop.hpp"
#include "IOHandler.hpp"

namespace io {
	bool EventLoop::add_fd( int fd, uint32_t events, IOHandler* handler ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;
		return 0 < epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
	}

	bool EventLoop::mod_fd( int fd, uint32_t events, IOHandler* handler ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;
		return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event) > 0;
	}

	bool EventLoop::del_fd( int fd ) {
		return 0 < epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
	}
}
