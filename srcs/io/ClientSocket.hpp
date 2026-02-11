#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
namespace io {

    class ClientSocket {
        private:
        int client_fd;
        public:
        ClientSocket() {
            client_fd = -1;
        }
        int accept_socket( int listen_fd ) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len;
            client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
            return client_fd;
        }
    };
}