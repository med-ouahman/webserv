
#include "Connection.hpp"

namespace core {


	bool Connection::advance( void ) {
		bytes_in_buff = handler.produce(buff, SEND_CHUNK_SIZE);
		return bytes_in_buff > 0;
	}
}
