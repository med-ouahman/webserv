
#include "Connection.hpp"

namespace core {

	bool Connection::advance( void ) {
		bytes_in = dispatcher.produce(writebuff, SEND_CHUNK_SIZE);
		if (bytes_in < 0) {
			state = ConnectionState::CLOSING;
			return false;
		} else if (bytes_in == 0) {
			if (bytes_received == 0) {
				state = ConnectionState::IDLE;
			} else {
				state = ConnectionState::READING;
			}
			return false;
		}
		return true;
	}
}
