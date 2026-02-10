#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>

namespace io {

    EventLoop::EventLoop(): epollFd(-1) {
        epollFd = epoll_create1(0);
        assert(epollFd >= 0);
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

    void EventLoop::add_connection( core::Connection* conn ) {
        struct epoll_event event;
        event.data.ptr = static_cast<void*>(conn);
        event.events = EPOLLIN;
        int result = epoll_ctl(epollFd, EPOLL_CTL_ADD, conn->get_fd(), &event);
        assert(result == 0);
        
        if (result != 0) {
            throw std::runtime_error("Failed to add connection to epoll");
        }

        events.push_back(event);
    }

    void EventLoop::run() {
        int n = 3;
        while (true) {
            std::cout << "Number of fds ready: " << n <<" \n"; 
            break;
        }
    }
}
