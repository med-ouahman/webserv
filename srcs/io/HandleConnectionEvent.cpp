
#include "Connection.hpp"
#include "EventLoop.hpp"

#define BUFFER_SIZE 8192

namespace io {

    void EventLoop::read_from_socket( core::Connection& conn ) {
        ssize_t bytes;
        char buff[BUFFER_SIZE];
        int n(0);
        while ((bytes = read(conn.get_fd(), buff, BUFFER_SIZE - 1)) > 0) {
            buff[bytes] = 0;
            n++;
            std::cout << "Recieved chunk (" << n << ")\n" << buff << std::endl;
            if (!conn.on_bytes(buff)) {
                break;
            }
        }
    }

    void EventLoop::write_to_socket( core::Connection& conn ) {
        char buff[BUFFER_SIZE];
        ssize_t bytes;
        while ((bytes = conn.peek_bytes(buff, BUFFER_SIZE)) > 0) {
            write(conn.get_fd(), buff, bytes);
        }
    }
}
