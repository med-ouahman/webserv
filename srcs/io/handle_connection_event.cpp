
#include "Connection.hpp"
#include "EventLoop.hpp"
#include <string.h>
#include <cerrno>

namespace io {
    void EventLoop::read_from_socket( core::Connection& conn ) {
        ::ssize_t bytes;

        while (true) {
            if (conn.read_buff_empty()) {
                bytes = ::read(conn.get_fd(), conn.get_read_buff(), core::Connection::READ_BUFFER_SIZE);
                if (!conn.on_read(bytes)) {
                    break;
                }
            }
            
            if (!conn.on_bytes()) {
                break;
            }
        }
        
    }

    void EventLoop::write_to_socket( core::Connection& conn ) {
        
        ::ssize_t bytes_sent = 0;
        
        while (true) {
            if (!conn.on_write(bytes_sent)) {
                break;
            }

            bytes_sent = ::write(conn.get_fd(),
                conn.get_write_buff(),
                conn.bytes_remaining()
            );
        }
    }
}