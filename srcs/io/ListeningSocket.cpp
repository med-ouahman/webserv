#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include "EventType.hpp"

namespace io {

	ListeningSocket::ListeningSocket( const ListeningSocket& socket ): loop(socket.loop) {
		(void)socket;
	}

	ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		return *this;
	}

	ListeningSocket::ListeningSocket( EventLoop& loop ): socket_fd(-1), loop(loop) {
	
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
}