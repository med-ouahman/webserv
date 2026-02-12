

#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

namespace io {
    bool ListeningSocket::accept_clients() {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;

        while (true) {
            int client_fd = accept(server_fd, (struct sockaddr* )&client_addr, &client_addr_len);
            if (client_fd < 0) {
                return false;
            }
            std::cout << client_fd << '\n';
            loop.add_connection(client_fd);
        }
        return true;
    }

    bool ListeningSocket::on_error() {
        return false;
    }
}
