#include "Connection.hpp"

namespace core {

	void Connection::handle_event() {
		
		switch (io_event) {
			case io::NONE:
				break;
			case io::READABLE:
				std::cout << "READING EVENT\n";
				state = ConnectionState::READING;
				break;
			case io::WRITABLE:
				std::cout << "WRITING EVENT\n";
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
