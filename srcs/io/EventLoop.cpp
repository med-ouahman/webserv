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

    void handleConnection( struct epoll_event& event, int listenFd ) {
        core::Connection* conn = (core::Connection* )&event.data;
        // handle EPOLLIN, EPOLLOUT, EPOLLHUB...

        int fd = conn->get_fd();
        if ( fd == listenFd ) {
            // accept a new connection
        } else {
            // read, write
        }
    }

    void EventLoop::run() {
	    ListeningSocket s;
        
	    int listenFd = s.getFd();
        add_fd(listenFd, EPOLLIN, listenFd);
        int clientFd = -1;
        while (running) {
            int n = epoll_wait(epollFd, &events.at(0), events.size(), -1);
            if (n < 0) {
                continue;
            }
            for ( int i = 0; i < n; i++ ) {
                
                handleConnection(events.at(i), listenFd);
            }

            break;
        }
    }
}
