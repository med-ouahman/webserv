
#include "EventPoller.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include "Result.hpp"

namespace runtime {
namespace epoll {

EventPoller::EventPoller()
    : epoll_fd(-1),
    created_(false),
    monitor_count(0) {

    epoll_fd = epoll_create(MaxMonitorFds);

    if (epoll_fd < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));
        return;
    }
  
    int flags = ::fcntl(epoll_fd, F_GETFD);
    
    if (flags < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));
        return;
    }
    
    if (::fcntl(epoll_fd, F_SETFD, flags | O_CLOEXEC) < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventPoller::epoll_create()"));
        return;
    }
    
    created_ = true;
}

bool EventPoller::created() const {
    return created_;
}

EventPoller::~EventPoller() {

    if (epoll_fd < 0) return;
    
    ::close(epoll_fd);
    epoll_fd = -1;
}

io::Event EventPoller::encode_events(EpollEvent epoll_event) {
    io::Event event = io::None;

    if (epoll_event & EPOLLIN)
        event = (io::Event)(event | io::Readable);

    if (epoll_event & EPOLLOUT)
        event = (io::Event)(event | io::Writable);

    if (epoll_event & EPOLLHUP)
        event = (io::Event)(event | io::Hup);

    if (epoll_event & EPOLLRDHUP)
        event = (io::Event)(event | io::RHup);

    if (epoll_event & EPOLLERR)
        event = (io::Event)(event | io::Error);

    return event;
}

EpollEvent EventPoller::decode_events(io::Event event) {
    EpollEvent ev = 0;

    if (event & io::Readable)
        ev |= EPOLLIN;

    if (event & io::Writable)
        ev |= EPOLLOUT;

    if (event & io::Hup)
        ev |= EPOLLHUP;

    if (event & io::RHup)
        ev |= EPOLLRDHUP;

    if (event & io::Error)
        ev |= EPOLLERR;

    return ev;
}


}}
