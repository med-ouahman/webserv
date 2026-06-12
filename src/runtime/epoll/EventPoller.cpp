
#include "EventPoller.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include "Result.hpp"

namespace runtime {
namespace epoll {

EventPoller::EventPoller()
    : epoll_fd(-1),
    monitor_count(0) {

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd < 0) LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));
    return;
    int flags = ::fcntl(epoll_fd, F_GETFD);
    
    if (flags < 0)  LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));
    
    if (::fcntl(epoll_fd, F_SETFD, flags | O_CLOEXEC) < 0) LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));

}

EventPoller::~EventPoller() {

    if (epoll_fd < 0) return;
    
    ::close(epoll_fd);
    epoll_fd = -1;
}

}}
