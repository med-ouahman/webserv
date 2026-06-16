
#include "EventPoller.hpp"
#include "Connection.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include "Result.hpp"

namespace runtime {
namespace epoll {

int EventPoller::poll() {
    
    int n = ::epoll_wait(epoll_fd, events, MaxEvents, EpollMaxTimeoutMs);
    if (n < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::run()"));
        return 1;
    }

    std::cout << "EPOLL_WAIT STARTS HERE-----------------------------------------------------------------\n";
    for (int i(0); i < n; ++i) {
        io::AEventHandler* handler = static_cast<io::AEventHandler*>(events[i].data.ptr); 
        std::cout << "EVENT_HANDLER: " << handler << "\n";
        switch (events[i].events) {
            case EPOLLIN:
                std::cout << "EPOLLIN\n";
                break;
            case EPOLLOUT:
                std::cout << "EPOLLOUT\n";
                break;
            case EPOLLHUP:
                std::cout << "HUP\n";
                break; 
            case EPOLLRDHUP:
                std::cout << "RHUP\n";
                break;
        }

        handler->on_event(encode_events(events[i].events));
    }
    std::cout << "EPOLL_WAIT ENDS HERE-----------------------------------------------------------------\n";
    return 0;
}

}
}
