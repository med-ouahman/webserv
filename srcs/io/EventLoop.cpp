#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "ListeningSocket.hpp"

namespace io {

    EventLoop::EventLoop( const config::Config& conf ): epollFd(-1), running(false), conf(conf) {
        epollFd = epoll_create1(0);
        assert(epollFd >= 0);
    }

    EventLoop::~EventLoop() {
        running = false;
        if (epollFd != -1) {
            close(epollFd);
        }

        for ( size_t i = 0; i < conns.size(); ++i ) {
            delete conns[i];
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
