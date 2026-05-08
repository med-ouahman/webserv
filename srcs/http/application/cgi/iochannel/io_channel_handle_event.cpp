#include "IOChannel.hpp"
#include "CGIHandler.hpp"

namespace http {

	std::string IOChannel::channel_type( STDStream::Type S ) {

		switch (S) {
			case STDStream::STDERR:
				return "STDERR";
			case STDStream::STDIN:
				return "STDIN";
			case STDStream::STDOUT:
				return "STDOUT";
		}
		return "ERROR TYPE";
	}

	void IOChannel::handle_event() {
		
		std::string c = channel_type(stream);
		std::cout << "Event: " << c << "\n";
		switch (io_event) {
			case io::NONE:
				break;
			case io::READABLE:
				processing = true;
				break;
			case io::WRITABLE:
				processing = true;
				break;
			case io::ERROR:
				listener->on_error();
				break;
		}

	}

}
