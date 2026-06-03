#include "EventPoller.hpp"
#include "Connection.hpp"
#include <iostream>
#include "Listener.hpp"
#include <fcntl.h>
#include <sys/epoll.h>

namespace runtime {
namespace epoll {

EventPoller::EventPoller()
    : epoll_fd(-1) {

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));
}

EventPoller::~EventPoller() {

    if (epoll_fd < 0) return;
    
    ::close(epoll_fd);
    epoll_fd = -1;
}

}}
