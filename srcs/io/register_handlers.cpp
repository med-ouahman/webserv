
#include "EventLoop.hpp"
#include "CGIHandler.hpp"

namespace io {
 
    void EventLoop::register_io_handler( IIOHandler* io_handler ) {
        pending_handlers.push_back(io_handler);
    }

    void EventLoop::register_cgi_handler( http::CGIHandler* cgi_handler ) {
        cgi_handlers.push_back(cgi_handler);
    }
    
}