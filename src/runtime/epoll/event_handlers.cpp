#include "EventPoller.hpp"
#include "AEventHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include "Result.hpp"

namespace runtime {

namespace epoll {

bool EventPoller::add(io::AEventHandler* handler) {
	if (monitor_count >= MaxMonitorFds) {
		std::cerr << "Cannot add fd, limit reached\n";
		return false;
	}

	epoll_event event;
	event.events = decode_events(handler->events());
	event.data.ptr = handler;
	
	int flags = ::fcntl(handler->fd(), F_GETFL);
	if (flags < 0 || ::fcntl(handler->fd(), F_SETFL, flags | O_NONBLOCK)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::add::fcntl()"));
		return false;
	}
	
	flags = ::fcntl(handler->fd(), F_GETFD);
	if (flags < 0 || ::fcntl(handler->fd(), F_SETFD, flags | O_CLOEXEC)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::add::fcntl()"));
		return false;
	}
	
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->fd(), &event)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::add::epoll_ctl(EPOLL_CTL_ADD)"));
		return false;
	}
	
	#ifdef DEBUG
	std::cout << "REGISTERED FD: " << handler->fd() << "\n";
	#endif

	++monitor_count;
	return true;
}

bool EventPoller::mod(io::AEventHandler* handler) {
	
	epoll_event event;
	event.events = decode_events(handler->events());
	event.data.ptr = const_cast<io::AEventHandler*>(handler);
	
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, handler->fd(), &event) < 0) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::mod::epoll_ctl(EPOLL_CTL_MOD)"));
		return false;
	}
	
	#ifdef DEBUG 
	std::cout << "MODIFIED FD: " << handler->fd() << "\n";
	#endif
	return true;
}

bool EventPoller::del(io::AEventHandler* handler) {
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, handler->fd(), NULL)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::del::epoll_ctl(EPOLL_CTL_DEL)"));
		return false;
	}

	#ifdef DEBUG
	std::cout << "DELETED FD: " << handler->fd() << std::endl;
	#endif
	// --monitor_count;
	return true;
}


void EventPoller::sync(io::AEventHandler* handler) {
	
	if (handler->synced())
		return;

	if (handler->events() == io::Close) {
		del(handler);
		return;
	}

	mod(handler);
	handler->sync_events();
}

io::Event EventPoller::encode_events(EpollEvent epoll_event) {
    io::Event event = io::None;

    if (epoll_event & EPOLLIN)
        event = (io::Event)(event | io::Readable);

    if (epoll_event & EPOLLOUT)
        event = (io::Event)(event | io::Writable);

    if (epoll_event & EPOLLHUP)
        event = (io::Event)(event | io::Hup);

    if (epoll_event & EPOLLRDHUP)
        event = (io::Event)(event | io::RHup);

    if (epoll_event & EPOLLERR)
        event = (io::Event)(event | io::Error);

    return event;
}

EpollEvent EventPoller::decode_events(io::Event event) {
    EpollEvent ev = 0;

    if (event & io::Readable)
        ev |= EPOLLIN;

    if (event & io::Writable)
        ev |= EPOLLOUT;

    if (event & io::Hup)
        ev |= EPOLLHUP;

    if (event & io::RHup)
        ev |= EPOLLRDHUP;

    if (event & io::Error)
        ev |= EPOLLERR;

    return ev;
}



}}
