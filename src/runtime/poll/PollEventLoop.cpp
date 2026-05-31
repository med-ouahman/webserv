#include "PollEventLoop.hpp"

namespace runtime {
namespace poll {

PollEventLoop::PollEventLoop() {

}

PollEventLoop::~PollEventLoop() {

}

void PollEventLoop::add_fd(const pollfd* fd) {

    if (nfds >= MAX_FDS) return;
    ++nfds;
    fds[nfds].events = fd->events;
    fds[nfds].fd = fd->fd;
    fds[nfds].revents = fd->revents;
}

int PollEventLoop::run() {
    while (true) {
       int n = ::poll(fds, nfds, 1000);

       if (n < 0) return 1;
    
    }
}

}
}
