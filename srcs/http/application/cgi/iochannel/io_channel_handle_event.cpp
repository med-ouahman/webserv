#include "IOChannel.hpp"
#include "CGIHandler.hpp"

namespace http {

	void IOChannel::handle_event() {
		
		switch (io_event) {
			case io::NONE:
				break;
			case io::READABLE:
				on_readable();
				break;
			case io::WRITABLE:
				on_writeable();
				break;
			case io::ERROR:
				listener->on_error();
				break;
		}
	}

}
