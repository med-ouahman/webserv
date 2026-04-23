
#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {

    int EventLoop::run( void ) {

        struct epoll_event events[MAX_EVENTS];
        running = start_listeners();
        while (running) {
            int n = ::epoll_wait(epoll_fd, events, MAX_EVENTS, MAX_TIMEOUT_MS);
            if (n < 0) {
                LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::run()"));
                return 1;
            }

            for ( int i(0); i < n; ++i ) {
                IOHandler* handler = static_cast<IOHandler*>(events[i].data.ptr);
                if (events[i].events & EPOLLIN) {
                    handler->on_event(READABLE);
                } else if (events[i].events & EPOLLOUT) {
                    handler->on_event(WRITABLE);
                } else if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    handler->on_event(ERROR);
                }
            }

            for ( ::size_t i(0); i < conns.size(); ++i ) {
                while (apply_connection_actions(conns.at(i)));
                update_epoll_interest(conns.at(i));
            }
            
            /*
                scans the connections to see those which want to close.
                we could have done it in the loop above. but that would result in address curroption
                we mark connections as closed by setting their state to CLOSING.
            */

            remove_connections();
        }
        return !running;
    }
}
