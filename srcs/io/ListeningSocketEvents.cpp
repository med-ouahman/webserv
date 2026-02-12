

#include "ListeningSocket.hpp"
#include "Connection.hpp"

namespace io {
    bool ListeningSocket::accept_clients() {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;

        while (true) {
            int client_fd = accept(server_fd, (struct sockaddr* )&client_addr, &client_addr_len);
            if (client_fd < 0)
                return false;
            loop.add_connection(client_fd);
        }
        return true;
    }

    bool ListeningSocket::on_error() {
        
    }
}
