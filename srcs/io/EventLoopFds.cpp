#include "EventLoop.hpp"
#include "IOHandler.hpp"

namespace io {
	bool EventLoop::add_fd( int fd, uint32_t events, IOHandler* handler ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
			return false;
		}
		return true;
	}

	bool EventLoop::mod_fd( int fd, uint32_t events, IOHandler* handler ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event)) {
			return false;
		}
		return true;
	}

	bool EventLoop::del_fd( int fd ) {
		
		if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL)) {
			return false;
		}
		return true;
	}
}
