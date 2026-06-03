#include "EventPoller.hpp"
#include "AEventHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include "Result.hpp"

namespace runtime {

namespace epoll {

bool EventPoller::register_handler(io::AEventHandler* handler) {
	epoll_event event;
	event.events = decode_events(handler->events());
	event.data.ptr = handler;
	
	int flags = ::fcntl(handler->fd(), F_GETFL);
	if (flags < 0 || ::fcntl(handler->fd(), F_SETFL, flags | O_NONBLOCK)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::add_connection::fcntl()"));
		return false;
	}
	
	flags = ::fcntl(handler->fd(), F_GETFD);
	if (flags < 0 || ::fcntl(handler->fd(), F_SETFD, flags | O_CLOEXEC)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::add_connection::fcntl()"));
		return false;
	}
	
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->fd(), &event)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_ctl(EPOLL_CTL_ADD)"));
		return false;
	}
	
	#ifdef DEBUG
	std::cout << "REGISTERED FD: " << handler->fd() << "\n";
	#endif
	return true;
}

bool EventPoller::modify_handler(io::AEventHandler* handler) {
	
	epoll_event event;
	event.events = decode_events(handler->events());
	event.data.ptr = const_cast<io::AEventHandler*>(handler);
	
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, handler->fd(), &event) < 0) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_ctl(EPOLL_CTL_MOD)"));
		return false;
	}
	
	#ifdef DEBUG 
	std::cout << "MODIFIED FD: " << handler->fd() << "\n";
	#endif
	return true;
}

bool EventPoller::del_handler(io::AEventHandler* handler) {
	if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, handler->fd(), NULL)) {
		LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_ctl(EPOLL_CTL_DEL)"));
		return false;
	}

	#ifdef DEBUG
	std::cout << "DELETED FD: " << handler->fd() << std::endl;
	#endif
	return true;
}


void EventPoller::sync(io::AEventHandler* handler) {
	
	if (handler->synced())
		return;

	if (handler->events() == io::ERROR) {
		del_handler(handler);
		return;
	}

	modify_handler(handler);
	handler->sync_events();
}

io::Event EventPoller::encode_events(EpollEvent epoll_event) {
    io::Event event = io::NONE;

    if (epoll_event & EPOLLIN)
        event = (io::Event)(event | io::READABLE);

    if (epoll_event & EPOLLOUT)
        event = (io::Event)(event | io::WRITABLE);

    if (epoll_event & EPOLLHUP)
        event = (io::Event)(event | io::HUP);

    if (epoll_event & EPOLLRDHUP)
        event = (io::Event)(event | io::RHUP);

    if (epoll_event & EPOLLERR)
        event = (io::Event)(event | io::ERROR);

    return event;
}

EpollEvent EventPoller::decode_events(io::Event event) {
    EpollEvent ev = 0;

    if (event & io::READABLE)
        ev |= EPOLLIN;

    if (event & io::WRITABLE)
        ev |= EPOLLOUT;

    if (event & io::HUP)
        ev |= EPOLLHUP;

    if (event & io::RHUP)
        ev |= EPOLLRDHUP;

    if (event & io::ERROR)
        ev |= EPOLLERR;

    return ev;
}



}}
