

#include "ListeningSocket.hpp"
#include <sys/socket.h>
#include <netinet/in.h>

namespace net {
    
    bool ListeningSocket::accept_clients() {

        struct sockaddr_in client_addr;
        
        socklen_t client_addr_len = sizeof(client_addr);
    
        int client_fd = ::accept(fd(), (struct sockaddr* )&client_addr, &client_addr_len);
        
        if (client_fd < 0) return false;
        

        if (callback_) callback_(client_fd, context_);

        std::cout << "CONNECTION_FD: " << client_fd << "\n";
        
        return true;
    }

    bool ListeningSocket::on_error() {
        LOG_ERROR(MAKE_ERRNO_ERROR("EventLoop::accept()"));
        return false;
    }
}
