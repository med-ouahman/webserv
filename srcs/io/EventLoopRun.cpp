
#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {

    void EventLoop::run( void ) {
        
        running = true;
        start_listeners();
        while (running) {

            uint32_t ev_flags;
            int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
       
            for ( int i = 0; i < n; i++ ) {
                IOHandler* handler = static_cast<IOHandler*>(events[i].data.ptr);
                std::cout << handler << "\n";
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
