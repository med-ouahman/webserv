#include "EventLoop.hpp"
#include "Connection.hpp"
#include "ListeningSocket.hpp"
#include <sys/socket.h>
#include <stdio.h>
namespace io {

    void EventLoop::run( ListeningSocket& server ) {
        
        add_fd(server.getFd(), EPOLLIN, &server);
        
        while (running) {
            int n = epoll_wait(epollFd, events, MAX_EVENTS, -1);
            std::cout << n << '\n';
            for ( int i = 0; i < n; i++ ) {
                IOHandler* handler = static_cast<IOHandler*>(events[i].data.ptr);
                handler->on_event(events[i].events);
            }
        }
    }
}