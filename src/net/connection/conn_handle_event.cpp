#include "Connection.hpp"

namespace net {

	void Connection::handle_event() {

		switch (io_event) {
			case io::NONE:
				break;
			case io::READABLE:
				std::cout << "READING EVENT\n";
				state = ConnectionState::READING;
				session.consume(data_view);
				break;
			case io::WRITABLE:
				std::cout << "WRITING EVENT\n";
				session.produce(writer);
				state = ConnectionState::WRITING;
				break;
			case io::ERROR:
				state = ConnectionState::CLOSING;
				break;
			default:
				break;
		}
	}
}
