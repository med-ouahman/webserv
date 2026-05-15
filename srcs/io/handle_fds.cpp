#include "EventLoop.hpp"
#include "AIOHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>

namespace io {
	bool EventLoop::add_fd( int fd, uint32_t events, AIOHandler* handler ) const {
		
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;

        int flags = ::fcntl(fd, F_GETFL);
        if (flags < 0 || ::fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::add_connection::fcntl()"));
            return false;
        }

        flags = ::fcntl(fd, F_GETFD);
        if (flags < 0 || ::fcntl(fd, F_SETFD, flags | O_CLOEXEC)) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::add_connection::fcntl()"));
            return false;
        }

		if (::epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_ADD)"));
			return false;
		}

		#ifdef DEBUG
			std::cout << "REGISTERED FD: " << fd << "\n";
		#endif
		return true;
	}

	bool EventLoop::mod_fd( int fd, uint32_t events, AIOHandler* handler ) const {
		epoll_event event;
		event.events = events;
		event.data.ptr = handler;

		if (::epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_MOD)"));
			return false;
		}
	
		#ifdef DEBUG 
			std::cout << "MODIFIED FD: " << fd << "\n";
		#endif
		return true;
	}

	bool EventLoop::del_fd( int fd ) const {
		
		if (::epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL)) {
			LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_ctl(EPOLL_CTL_DEL)"));
			return false;
		}

		#ifdef DEBUG
			std::cout << "DELETED FD: " << fd << std::endl;
		#endif
		
		return true;
	}
}
