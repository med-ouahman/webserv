#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "ListeningSocket.hpp"

namespace io {

    EventLoop::EventLoop( const config::Config& conf ): epoll_fd(-1), running(false), conf(conf) {
        
        epoll_fd = epoll_create1(0);
        running = true;
        if (epoll_fd < 0) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
            running = false;
        }
    }

    EventLoop::~EventLoop() {
        running = false;
        if (epoll_fd != -1) {
            close(epoll_fd);
        }

        for ( ::size_t i = 0; i < conns.size(); ++i ) {
            delete conns[i];
        }
        
        for ( ::size_t i = 0; i < listeners.size(); i++ ) {
            listeners[i].close();
        }
    }

    EventLoop& EventLoop::operator=( const EventLoop& other ) { 
        if (this != &other) {}
        return *this;
    }

    EventLoop::EventLoop( const EventLoop& other ): conf(other.conf) {
        (void)other;
    }

}
