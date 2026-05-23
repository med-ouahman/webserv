
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>

namespace runtime {

namespace epoll {

int EventLoop::run() {
    
    if (epoll_fd < 0) return EXIT_FAILURE;

    struct epoll_event events[Limits::MAX_EVENTS];
    while (true) {
        
        int n = ::epoll_wait(epoll_fd, events, Limits::MAX_EVENTS, Limits::EPOLL_WAIT_TIMEOUT_MS);
        if (n < 0) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::run()"));
            return EXIT_FAILURE;
        }
        
        for ( int i(0); i < n; ++i ) {
            io::AEventHandler* handler = static_cast<io::AEventHandler*>(events[i].data.ptr); 
            if (events[i].events & EPOLLIN) {
                handler->on_event(io::READABLE);
            } else if (events[i].events & EPOLLOUT) {
                handler->on_event(io::WRITABLE);
            } else if (events[i].events & EPOLLERR) {
                handler->on_event(io::ERROR);
            } else if (events[i].events & (EPOLLHUP | EPOLLRDHUP)) handler->on_event(io::HUP);
        }
    }
    
    return EXIT_FAILURE;
}

}
}
