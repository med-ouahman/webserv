#include "ListeningSocket.hpp"
#include "EventType.hpp"

class Server;
namespace net {

	ListeningSocket::ListeningSocket( const ListeningSocket& socket ): AEventHandler(socket.fd(), socket.mask()){
		
	}

	ListeningSocket& ListeningSocket::operator=( const ListeningSocket& socket ) {
		(void)socket;
		
		return *this;
	}

	ListeningSocket::ListeningSocket( int fd, io::EventMask mask, AcceptCallback cb, AcceptContext* ctx )
		: AEventHandler(fd, mask),
		callback_(cb),
		context_(ctx) {}

	ListeningSocket::~ListeningSocket() {

	}

	void ListeningSocket::on_event( io::EventType event ) {
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
