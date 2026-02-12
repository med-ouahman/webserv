#include "EventLoop.hpp"
#include "Connection.hpp"
#include "ListeningSocket.hpp"

namespace io {

    void EventLoop::run( ListeningSocket& server ) {
        add_fd(server.getFd(), EPOLLIN, &server);
        
        while (running) {
            int n = epoll_wait(epollFd, events, MAX_EVENTS, -1);
            for ( int i = 0; i < n; i++ ) {
                IOHandler* handler = static_cast<IOHandler*>(events[i].data.ptr);
                handler->on_event(events[i].events);
            }
        }
    }
}