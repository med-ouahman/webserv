
#pragma once

#include <sys/epoll.h>

namespace runtime {
namespace epoll {

enum Event {
    WRITABLE = EPOLLOUT,
    READABLE = EPOLLIN,
    HUP = EPOLLHUP,
    RHUP = EPOLLRDHUP,
    ERROR = EPOLLERR,
    };
}
}

