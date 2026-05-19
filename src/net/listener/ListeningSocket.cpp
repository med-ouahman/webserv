#include "ListeningSocket.hpp"
#include "EventLoop.hpp"
#include "EventType.hpp"

namespace io {

	ListeningSocket::ListeningSocket( const ListeningSocket& socket ): AEventHandler(socket.fd_), loop(socket.loop) {
		
	}

	ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		
		return *this;
	}

	ListeningSocket::ListeningSocket( EventLoop& loop, int fd ): AEventHandler(fd), loop(loop) {
	
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

	void ListeningSocket::release() {
		fd_ = -1;
	}
}