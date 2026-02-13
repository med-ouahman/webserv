#include "EventLoop.hpp"
#include "Connection.hpp"
#include "ListeningSocket.hpp"
#include <sys/socket.h>
#include <stdio.h>
namespace io {

    void EventLoop::run( ListeningSocket& server ) {
        
        add_fd(server.get_fd(), EPOLLIN | EPOLLET, &server);

        uint32_t ev_flags;
        running = true;
        while (running) {
            int n = epoll_wait(epollFd, events, MAX_EVENTS, -1);
            std::cout << "Events: " << n << std::endl;    
            for ( int i = 0; i < n; i++ ) {
                IOHandler* handler = static_cast<IOHandler*>(events[i].data.ptr);
                ev_flags = events[i].events;
                if (ev_flags & EPOLLIN) {
                    handler->on_event(READABLE);
                } else if (ev_flags & EPOLLOUT) {
                    handler->on_event(WRITABLE);
                } else if (ev_flags & (EPOLLERR | EPOLLHUP)) {
                    handler->on_event(ERROR);
                }
            }
            
            for ( std::vector<core::Connection>::iterator it = conns.begin(); it != conns.end(); it++ ) {
                
                core::ConnectionAction action = (*it).desired_action();
                if (action.want_close) {
                    // do the closing
                    // call remove_connection
                } else if (action.want_read) {
                    // perform reading
                } else if (action.want_write) {
                    // perform writing
                }
            }
        }
    }
}