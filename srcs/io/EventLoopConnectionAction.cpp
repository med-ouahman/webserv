#include "EventLoop.hpp"
#include "Connection.hpp"

namespace io {

    void EventLoop::write_to_socket( core::Connection& conn ) {
        
    }

    void EventLoop::read_form_socket( core::Connection& conn ) {
        int fd = conn.get_fd();

    }
}