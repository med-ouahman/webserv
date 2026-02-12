#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "ListeningSocket.hpp"

namespace io {

    EventLoop::EventLoop(): epollFd(-1), running(false) {
        epollFd = epoll_create1(0);
        assert(epollFd >= 0);
        running = true;
    }

    EventLoop::~EventLoop() {
    
        if (epollFd != -1) {
            close(epollFd);
        }

    }

    EventLoop& EventLoop::operator=( const EventLoop& other ) { 
        if (this != &other) {
        }
        return *this;
    }

    EventLoop::EventLoop( const EventLoop& other ) {
        (void)other;
    }
}
