#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "ListeningSocket.hpp"
#include <fcntl.h>
#include <sys/epoll.h>

namespace runtime {
namespace epoll {

    EventLoop::EventLoop(): epoll_fd(-1), running(false) {
        
        epoll_fd = epoll_create1(EPOLL_CLOEXEC);
        
        running = true;
        if (epoll_fd < 0) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
            running = false;
        }
        
    }
    
    EventLoop::~EventLoop() {
        running = false;
        
        if (epoll_fd > -1) {
            ::close(epoll_fd);
            epoll_fd = -1;
        }
    }
}
}
