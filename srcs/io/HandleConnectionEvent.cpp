
#include "Connection.hpp"
#include "EventLoop.hpp"

#define BUFFER_SIZE 8192

namespace io {

    void EventLoop::read_from_socket( core::Connection& conn ) {
        ssize_t bytes;
        char buff[BUFFER_SIZE];
        while ((bytes = read(conn.get_fd(), buff, BUFFER_SIZE - 1)) > 0) {
            buff[bytes] = 0;
            conn.on_bytes(buff, bytes);
        }
    }

    void EventLoop::write_to_socket( core::Connection& conn ) {
        char buff[BUFFER_SIZE];

        conn.get_bytes(buff, BUFFER_SIZE - 1);
        
    }

}
