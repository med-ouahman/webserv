
#include "EventLoop.hpp"
#include "Connection.hpp"

class Heap {

    private:
        std::vector<core::Connection*> heap;
    public:
        int min() {

            if (heap.size() == 0)
                return -1;
            return 100; // later;
        };
};


namespace io {

    int EventLoop::run() {
       
        struct epoll_event events[MAX_EVENTS];

        Heap heap;
        
        while (running) {
            int n = ::epoll_wait(epoll_fd, events, MAX_EVENTS, heap.min());
            if (n < 0) {
                LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::run()"));
                return 1;
            }

            for ( int i(0); i < n; ++i ) {
                IIOHandler* handler = static_cast<IIOHandler*>(events[i].data.ptr);
                if (events[i].events & EPOLLIN) {
                    handler->on_event(READABLE);
                } else if (events[i].events & EPOLLOUT) {
                    handler->on_event(WRITABLE);
                } else if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                    handler->on_event(ERROR);
                }

            }
        
            for ( size_t i(0); i < conns.size(); ++i ) {
                update_epoll_interest(conns.at(i));
            }

            pump();
            sweep();
            
        }
        
        return int(!running);
    }
}
