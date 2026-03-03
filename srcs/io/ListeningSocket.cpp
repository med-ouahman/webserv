#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include "EventType.hpp"

namespace io {

	ListeningSocket::ListeningSocket( const ListeningSocket& socket ): loop(socket.loop) {
		socket_fd = socket.socket_fd;
	}

	ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		return *this;
	}

	ListeningSocket::ListeningSocket( EventLoop& loop, int fd ): socket_fd(fd), loop(loop) {
	
	}

	ListeningSocket::~ListeningSocket() {

	}

	void ListeningSocket::on_event( io::EventType event ) {
		switch (event) {
			case READABLE:
				accept_clients();
				break;
			case ERROR:
				on_error();
			default:
				break;
		}
	};

	void ListeningSocket::close( void ) {
		if (socket_fd >= 0) {
			::close(socket_fd);
			socket_fd = -1;
		}
	}
}