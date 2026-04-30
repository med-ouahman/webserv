
#include "Connection.hpp"

namespace core {

	bool Connection::advance( void ) {
		
		ssize_t p_bytes = dispatcher.produce(writebuff, SEND_CHUNK_SIZE);

		if (p_bytes < 0) {
			state = ConnectionState::CLOSING;
			return false;
		}
		
		bytes_to_write = p_bytes;
		
		return true;
	}
}
