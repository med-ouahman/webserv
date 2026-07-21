
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include "Result.hpp"

namespace runtime {
namespace epoll {

int EventLoop::poll() {

    int nfds = ::epoll_wait(epoll_fd.get(), events, MaxEvents, EpollMaxTimeoutMs);

    if (nfds < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::run()"));
        return 1;
    }

    for (int i(0); i < nfds; ++i) {
        io::AEventHandler* handler = static_cast<io::AEventHandler*>(events[i].data.ptr); 
        handler->on_event(encode_events(events[i].events));
    }

    return 0;
}

}
}
