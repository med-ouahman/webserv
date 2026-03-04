
#include "Connection.hpp"
#include "EventLoop.hpp"

namespace io {

    void EventLoop::read_from_socket( core::Connection& conn ) {
        ssize_t bytes;
        char buff[READ_BUFFER_SIZE];
        int n=0;
        while ((bytes = ::read(conn.get_fd(), buff, READ_BUFFER_SIZE - 1)) > 0) {

            buff[bytes] = n;
            std::cout << buff;
            if (!conn.on_bytes(buff)) {
                break;
            }
        }
    }

    void EventLoop::write_to_socket( core::Connection& conn ) {
        
        ssize_t bytes_sent = 0;
        
        while (true) {
            
             if (!conn.has_data(bytes_sent)) {
                break;
            }

            bytes_sent = ::write(conn.get_fd(),
                conn.get_write_buff(),
                conn.bytes_remaining()
            );
            
        }

    }
}
