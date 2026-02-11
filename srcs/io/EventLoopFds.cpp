#include "EventLoop.hpp"

namespace io {
	void EventLoop::add_fd( int fd, uint32_t events, int _fd) {
		epoll_event event;
		event.events = events;
		event.data.fd = fd;
		epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
	}

	void EventLoop::add_fd( int fd, uint32_t events, void* ptr ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = ptr;
		epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
	}

	void EventLoop::mod_fd( int fd, uint32_t events, int _fd) {
		epoll_event event;
		event.events = events;
		event.data.fd = fd;
		epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
	}

	void EventLoop::mod_fd( int fd, uint32_t events, void* ptr ) {
		epoll_event event;
		event.events = events;
		event.data.ptr = ptr;
		epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
	}

	void EventLoop::del_fd( int fd ) {
		epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, NULL);
	}
}
