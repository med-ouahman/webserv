
#include "EventLoop.hpp"

namespace io {
 
    void EventLoop::register_handler( IIOHandler* new_handler ) {
        pending_handlers.push_back(new_handler);
    }
}