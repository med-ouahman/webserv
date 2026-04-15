
#include "Connection.hpp"

namespace core {


	bool Connection::advance( void ) {
		bytes_in_buff = handler.produce(output_buff, SEND_CHUNK_SIZE);
		if (bytes_in_buff < 0) {
			state = CLOSING;
			return false;
		} else if (bytes_in_buff == 0) {
			if (read_buff_drained) {
				state = IDLE;
			} else {
				state = READING;
			}
			return false;
		}
		return true;
	}
}
