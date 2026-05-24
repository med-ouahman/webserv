
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cstdlib>

namespace runtime {
namespace epoll {

int EventLoop::run() {
    
    if (epoll_fd < 0) return EXIT_FAILURE;

    struct epoll_event events[MAX_EVENTS];
    while (true) {
        
        int n = ::epoll_wait(epoll_fd, events, MAX_EVENTS, EPOLL_TIMEOUT_MS);
        std::cout << "Events: " << n << "\n";
        if (n < 0) {
            LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::run()"));
            return EXIT_FAILURE;
        }
        
        for ( int i(0); i < n; ++i ) {
            io::AEventHandler* handler = static_cast<io::AEventHandler*>(events[i].data.ptr); 
            handler->on_event(encode_events(events[i].events));
            sync(handler);
        }
    }    
    return EXIT_FAILURE;
}

}}
