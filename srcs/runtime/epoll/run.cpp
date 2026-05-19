
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>

namespace io {

    int EventLoop::run() {
       
        struct epoll_event events[Limits::MAX_EVENTS];

        size_t cycles = 0;
        while (running) {

            int n = ::epoll_wait(epoll_fd, events, Limits::MAX_EVENTS, Limits::EPOLL_WAIT_TIMEOUT_MS);
            if (n < 0) {
                LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::run()"));
                return 1;
            }
            
            for ( int i(0); i < n; ++i ) {
                AEventHandler* handler = static_cast<AEventHandler*>(events[i].data.ptr); 
                if (events[i].events & EPOLLIN) {
                    handler->on_event(READABLE);
                } else if (events[i].events & EPOLLOUT) {
                    handler->on_event(WRITABLE);
                } else if (events[i].events & EPOLLERR) {
                    handler->on_event(ERROR);
                } else if (events[i].events & (EPOLLHUP | EPOLLRDHUP)) handler->on_event(HUP);

            }
        
            for ( size_t i(0); i < conns.size(); ++i ) {
                update_epoll_interest(conns.at(i));
            }

            sweep();
            ++cycles;
        }
        
        return int(!running);
    }
}
