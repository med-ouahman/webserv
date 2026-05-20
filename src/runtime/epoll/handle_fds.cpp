#include "EventLoop.hpp"
#include "AEventHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>

namespace runtime {

	namespace epoll {

		bool EventLoop::register_handler( io::AEventHandler* handler ) const {
			
			epoll_event event;
			event.events = handler->mask();
			event.data.ptr = handler;
			
			int flags = ::fcntl(handler->fd(), F_GETFL);
			if (flags < 0 || ::fcntl(handler->fd(), F_SETFL, flags | O_NONBLOCK)) {
				LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::add_connection::fcntl()"));
				return false;
			}
			
			flags = ::fcntl(handler->fd(), F_GETFD);
			if (flags < 0 || ::fcntl(handler->fd(), F_SETFD, flags | O_CLOEXEC)) {
				LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::add_connection::fcntl()"));
				return false;
			}
			
			if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, handler->fd(), &event)) {
				LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_ADD)"));
				return false;
			}
			
			#ifdef DEBUG
			std::cout << "REGISTERED FD: " << fd << "\n";
			#endif
			return true;
		}
		
		bool EventLoop::modify_handler( io::AEventHandler* handler ) const {
			epoll_event event;
			event.events = handler->fd();
			event.data.ptr = handler;
			
			if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, handler->fd(), &event)) {
				LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_MOD)"));
				return false;
			}
			
			#ifdef DEBUG 
			std::cout << "MODIFIED FD: " << fd << "\n";
			#endif
			return true;
		}
		
		bool EventLoop::del_handler( io::AEventHandler* handler ) const {
			
			if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, handler->fd(), NULL)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_DEL)"));
			return false;
		}
		
		#ifdef DEBUG
		std::cout << "DELETED FD: " << fd << std::endl;
		#endif
		
		return true;
	}
}
}
