
#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {

    void EventLoop::run( ListeningSocket& server ) {
        
        add_fd(server.get_fd(), EPOLLIN | EPOLLET, &server);

        uint32_t ev_flags;

        running = true;
        
        while (running) {
            int n = epoll_wait(epollFd, events, MAX_EVENTS, -1);
          
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
            
            for ( size_t i = 0; i < conns.size(); i++ ) {
                apply_connection_actions(conns.at(i));
            }
        }
    }
}
