#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

namespace io {

    ListeningSocket::ListeningSocket( const ListeningSocket& socket ): loop(socket.loop) {
		(void)socket;
	}

    ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		return *this;
	}

    ListeningSocket::ListeningSocket( EventLoop& loop ): server_fd(-1), loop(loop) {
        struct sockaddr_in server_addr;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(8080); // later will be Config::port
        server_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
        assert(server_fd >= 0);
        int r = bind(server_fd, (struct sockaddr *)&server_addr, sizeof server_addr);
        assert(r >= 0);
        r = listen(server_fd, BACKLOG);
        assert(r >= 0);
        (void)r;
        std::cout << "Server listening on localhost port: " << 8080 << '\n';
	}

    void ListeningSocket::on_event( uint32_t events ) {
		if (events & (EPOLLERR | EPOLLHUP)) {
			on_error();
		} else if (events & EPOLLIN) {
			accept_clients();
		}
	};
}