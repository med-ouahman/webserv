#include "EventLoop.hpp"
#include "AEventHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include "Result.hpp"
#include "Server.hpp"
#include <sstream>

namespace runtime {

namespace epoll {

bool EventLoop::add(io::AEventHandler* handler, bool conf) {

	if (monitor_count >= MaxMonitorFds) {
		std::stringstream ss;
		ss << "Cannot watch for FD (" << handler->fd() << "), Limit reached";
		logger.log(logger::Warning, ss.str());
		return false;
	}

	epoll_event event;
	event.events = decode_events(handler->events());
	event.data.ptr = handler;
	
	if (conf && !conf_handler_fd(handler->fd()))
		return false;

	if (::epoll_ctl(epoll_fd.get(), EPOLL_CTL_ADD, handler->fd(), &event)) {
		logger.log(logger::Error, logger.make_errno_error("EventLoop::add::epoll_ctl(EPOLL_CTL_ADD)", __FILE__, __LINE__));
		return false;
	}

	std::stringstream ss;
	ss << "FD (" <<  handler->fd() << ") added to epoll";
	logger.log(logger::Info, ss.str() , true);
	++monitor_count;
	return true;
}

bool EventLoop::mod(io::AEventHandler* handler) {
	
	epoll_event event;
	event.events = decode_events(handler->events());
	event.data.ptr = const_cast<io::AEventHandler*>(handler);
	
	if (::epoll_ctl(epoll_fd.get(), EPOLL_CTL_MOD, handler->fd(), &event) < 0) {		
		logger.log(logger::Error, logger::Logger::make_errno_error("epoll_ctl(EPOLL_CTL_MOD)", __FILE__, __LINE__));
		return false;
	}
	
	std::stringstream ss;
	ss << "FD (" <<  handler->fd() << ") modified";
	logger.log(logger::Info, ss.str());
	return true;
}

bool EventLoop::del(io::AEventHandler* handler) {
	if (::epoll_ctl(epoll_fd.get(), EPOLL_CTL_DEL, handler->fd(), NULL)) {
		logger.log(logger::Error,
			logger.make_errno_error("EventLoop::del::epoll_ctl(EPOLL_CTL_DEL)", __FILE__, __LINE__));
		return false;
	}

	std::stringstream ss;
	ss << "FD (" <<  handler->fd() << ") deleted";
	logger.log(logger::Info, ss.str());
	--monitor_count;
	return true;
}


void EventLoop::sync(io::AEventHandler* handler) {

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
