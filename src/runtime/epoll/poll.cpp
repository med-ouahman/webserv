
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

    if (n == 0) std::cout << "No events\n";

    for (int i(0); i < n; ++i) {
        io::AEventHandler* handler = static_cast<io::AEventHandler*>(events[i].data.ptr); 

        handler->on_event(encode_events(events[i].events));
    }

    return 0;
}

}
}
