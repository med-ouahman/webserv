

#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

namespace io {
    bool ListeningSocket::accept_clients() {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
    
        int client_fd = ::accept(fd_, (struct sockaddr* )&client_addr, &client_addr_len);
        if (client_fd < 0) {
            return false;
        }

        std::cout << "CONNECTION_FD: " << client_fd << "\n";
        loop.add_connection(client_fd);
        return true;
    }

    bool ListeningSocket::on_error() {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::accept()"));
        return false;
    }
}
