#include "ListeningSocket.hpp"
#include "Event.hpp"

class Server;
namespace net {

	ListeningSocket::ListeningSocket( const ListeningSocket& socket ): AEventHandler(socket.fd(), socket.mask()){
		
	}

	ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		
		return *this;
	}

	ListeningSocket::ListeningSocket( int fd, io::EventMask mask, AcceptCallback cb, AcceptContext ctx )
		: AEventHandler(fd, mask),
		callback_(cb),
		context_(ctx) {}

	ListeningSocket::~ListeningSocket() {

	}

	void ListeningSocket::on_event( runtime::epoll::Event event ) {
		switch (event) {
			case io::READABLE:
				accept_clients();
				break;
			case io::ERROR:
				on_error();
			default:
				break;
		}
	};
}
