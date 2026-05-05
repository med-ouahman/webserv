#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "ListeningSocket.hpp"
#include <fcntl.h>

namespace io {

    EventLoop::EventLoop( const config::Config& conf ): epoll_fd(-1), running(false), conf(conf) {
        
        epoll_fd = epoll_create1(EPOLL_CLOEXEC);

        running = true;
        if (epoll_fd < 0) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
            running = false;
        }

        running = running && start_listeners();
    }

    EventLoop::~EventLoop() {
        running = false;
        
        if (epoll_fd > -1) {
            ::close(epoll_fd);
        }

        for ( ::size_t i = 0; i < conns.size(); ++i ) {
            delete conns[i];
        }
        
    }

}
