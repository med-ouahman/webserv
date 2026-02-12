#include "ListeningSocket.hpp"
#include "EventLoop.hpp"

namespace io {

    ListeningSocket::ListeningSocket( const ListeningSocket& socket ): loop(loop) {
		(void)socket;
	}

    ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		return *this;
	}

    ListeningSocket::ListeningSocket( EventLoop& loop ): fd(-1), loop(loop) {
        struct sockaddr_in server_addr;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = 8080;
        fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK, 0);
        assert(fd >= 0);
        int r = bind(fd, (struct sockaddr *)&server_addr, sizeof server_addr);
        assert(r >= 0);
        r = listen(fd, BACKLOG);
        assert(r >= 0);
        (void)r;
	}

    void ListeningSocket::on_event( uint32_t events ) {
		if (events & (EPOLLERR | EPOLLHUP)) {
			on_error();
		} else if (events & EPOLLIN) {
			accept_clients();
		}
	};
}