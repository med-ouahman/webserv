
#pragma once

#include <sys/epoll.h>

namespace io {
    enum EventType {
        WRITABLE = EPOLLOUT,
        READABLE = EPOLLIN,
        HUP = EPOLLHUP,
        RHUP = EPOLLRDHUP,
        ERROR = EPOLLERR,
    };
}

