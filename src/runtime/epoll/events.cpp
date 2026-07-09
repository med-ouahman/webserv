#include "EventPoller.hpp"
#include "AEventHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include "Result.hpp"
#include "Server.hpp"
#include <sstream>

namespace runtime {

namespace epoll {

bool EventPoller::add(io::AEventHandler* handler) {

	if (monitor_count >= MaxMonitorFds) {
		std::stringstream ss;
		ss << "Cannot watch for FD (" << handler->fd() << "), Limit reached";

		Server::logger().log(logger::Warning, ss.str() , true);
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

	std::stringstream ss;
	ss << "Watching for FD (" <<  handler->fd() << ")";
	Server::logger().log(logger::Info, ss.str() , true);
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
		std::cerr << "Fd: " << handler->fd() << "\n";
		return false;
	}

	#ifdef DEBUG
	std::cout << "DELETED FD: " << handler->fd() << std::endl;
	#endif
	--monitor_count;
	return true;
}


void EventPoller::sync(io::AEventHandler* handler) {

	if (handler->events() == io::Close) {
		del(handler);
		return;
	}

	if (handler->synced())
		return;

	mod(handler);
	handler->sync_events();
}

}}
