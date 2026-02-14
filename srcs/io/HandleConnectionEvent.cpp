
#include "Connection.hpp"
#include "EventLoop.hpp"

#define BUFFER_SIZE 8192

namespace io {

    void EventLoop::read_from_socket( core::Connection& conn ) {
        ssize_t bytes;
        char buff[BUFFER_SIZE];
        while ((bytes = read(conn.get_fd(), buff, BUFFER_SIZE - 1))) {
            
        }
    }

    void EventLoop::write_to_socket( core::Connection& conn ) {
        
    }

}
