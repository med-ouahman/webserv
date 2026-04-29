#include "EventLoop.hpp"
#include "IIOHandler.hpp"

namespace io {
	bool EventLoop::add_fd( int fd, uint32_t events, IIOHandler* handler ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;

		if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_ADD)"));
			return false;
		}

		return true;
	}

	bool EventLoop::mod_fd( int fd, uint32_t events, IIOHandler* handler ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;

		if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_MOD)"));
			return false;
		}
		
		return true;
	}

	bool EventLoop::del_fd( int fd ) {
		
		if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_DEL)"));
			return false;
		}
		
		return true;
	}
}
