
#include "Connection.hpp"

namespace core {


	bool Connection::advance( void ) {

		handler.produce(buff, SEND_CHUNK_SIZE);
		return true;
	}
}
