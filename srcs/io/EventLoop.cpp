#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "ListeningSocket.hpp"

namespace io {

    EventLoop::EventLoop( const config::Config& conf ): epoll_fd(-1), running(false), conf(conf) {
        epoll_fd = epoll_create1(0);
        assert(epoll_fd >= 0);
    }

    EventLoop::~EventLoop() {
        running = false;
        if (epoll_fd != -1) {
            close(epoll_fd);
        }

        for ( size_t i = 0; i < conns.size(); ++i ) {
            delete conns[i];
        }
        
        for ( size_t i = 0; i < listeners.size(); i++ ) {
            listeners[i].close_fd();
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
