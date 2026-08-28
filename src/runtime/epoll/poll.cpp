
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/epoll.h>
#include <sys/socket.h>
#include "Result.hpp"

namespace runtime {
namespace epoll {

bool EventLoop::poll() {

    int nfds = ::epoll_wait(epoll_fd.get(), events, MaxEvents, EpollMaxTimeoutMs);

    if (nfds < 0) {
        
        if (errno == EINTR) return true;
        logger.log(logger::Error, "EventLoop::epoll_wait()");
        return false;
    }

    for (int i(0); i < nfds; ++i) {
        io::AEventHandler* handler = static_cast<io::AEventHandler*>(events[i].data.ptr); 
        handler->on_event(encode_events(events[i].events));
    }

    return true;
}

}
}
