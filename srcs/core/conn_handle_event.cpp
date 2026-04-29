#include "Connection.hpp"

namespace core {

	void Connection::handle_event( void ) {
		switch (io_event) {
			case io::NONE:
				break;
			case io::READABLE:
				state = ConnectionState::READING;
				break;
			case io::WRITABLE:
				state = ConnectionState::READING;
				break;
			case io::ERROR:
				state = ConnectionState::CLOSING;
				break;
			default:
				break;
		}

		process();
	}
}
