#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {
    bool EventLoop::add_connection( int client_fd ) {
        core::Connection* conn = new core::Connection(client_fd);
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.ptr = conn;
        int r = epoll_ctl(epollFd, EPOLL_CTL_ADD, client_fd, &event);
        assert(r != -1);
        conns.push_back(*conn);
        return r == 0;
    }
}
