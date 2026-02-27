
#include "Connection.hpp"
#include "EventLoop.hpp"

#define BUFFER_SIZE 8192

namespace io {

    void EventLoop::read_from_socket( core::Connection& conn ) {
        ssize_t bytes;
        char buff[BUFFER_SIZE];
        int n(0);
        while ((bytes = ::read(conn.get_fd(), buff, BUFFER_SIZE - 1)) > 0) {
            buff[bytes] = 0;
            n++;
            std::cout << "Recieved chunk (" << n << ")\n" << buff;
            if (!conn.on_bytes(buff)) {
                break;
            }
        }
    }

    void EventLoop::write_to_socket( core::Connection& conn ) {
        
        ssize_t bytes_sent;
        while ((bytes_sent = write(conn.get_fd(), conn.get_write_buff().c_str(), conn.get_write_buff().size())) > 0) {
            conn.update_buff(bytes_sent);
        }
    }
}
