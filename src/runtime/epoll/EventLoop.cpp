#include "EventLoop.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Listener.hpp"
#include <fcntl.h>
#include <sys/epoll.h>

namespace runtime {
namespace epoll {

EventLoop::EventLoop( void* ctx, ServerCallback cb_)
    : server_ctx(ctx),
    server_callback(cb_),
    epoll_fd(-1) {

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
}

EventLoop::~EventLoop() {

    if (epoll_fd < 0) return;
    
    ::close(epoll_fd);
    epoll_fd = -1;
}

}}
