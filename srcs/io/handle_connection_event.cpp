
#include "Connection.hpp"
#include "EventLoop.hpp"

namespace io {
    int n = 0;
    void EventLoop::read_from_socket( core::Connection& conn ) {
        ssize_t bytes;
        char buff[READ_BUFFER_SIZE];
        std::cout << "reading\n";
        while (true) {
            bytes = ::read(conn.get_fd(), buff, READ_BUFFER_SIZE);

            write(1, buff, bytes < 0 ? 0: bytes);

            if (!conn.on_bytes(buff, bytes)) {
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