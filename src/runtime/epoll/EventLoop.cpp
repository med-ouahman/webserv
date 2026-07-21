
#include "EventLoop.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/epoll.h>
#include "Result.hpp"
#include "Server.hpp"

namespace runtime {
namespace epoll {

EventLoop::EventLoop()
    : epoll_fd(epoll_create(MaxMonitorFds)),
    created_(false),
    monitor_count(0),
    logger(Server::logger) {

    if (!epoll_fd.valid()) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
        return;
    }
  
    int flags = ::fcntl(epoll_fd.get(), F_GETFD);
    
    if (flags < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
        return;
    }
    
    if (::fcntl(epoll_fd.get(), F_SETFD, flags | O_CLOEXEC) < 0) {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::epoll_create()"));
        return;
    }
    
    created_ = true;
}

bool EventLoop::created() const {
    return created_;
}

EventLoop::~EventLoop() {

}


io::Event EventLoop::encode_events(EpollEvent epoll_event) {
    io::Event event = io::None;

    if (epoll_event & EPOLLIN) event = (io::Event)(event | io::Readable);

    if (epoll_event & EPOLLOUT) event = (io::Event)(event | io::Writable);

    if (epoll_event & EPOLLHUP) event = (io::Event)(event | io::Hup);

    if (epoll_event & EPOLLRDHUP) event = (io::Event)(event | io::RHup);

    if (epoll_event & EPOLLERR) event = (io::Event)(event | io::Error);

    return event;
}



EpollEvent EventLoop::decode_events(io::Event event) {
    EpollEvent ev = 0;

    if (event & io::Readable) ev |= EPOLLIN;

    if (event & io::Writable) ev |= EPOLLOUT;

    if (event & io::Hup) ev |= EPOLLHUP;

    if (event & io::RHup) ev |= EPOLLRDHUP;

    if (event & io::Error) ev |= EPOLLERR;

    return ev;
}

}}
