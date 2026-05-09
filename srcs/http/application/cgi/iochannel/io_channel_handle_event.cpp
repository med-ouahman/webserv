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

		if (state == IOChannelState::CLOSED)
			return ;
		state = IOChannelState::ACTIVE;
		switch (io_event) {
			case io::NONE:
				state = IOChannelState::IDLE;
				break;
			case io::READABLE:
				std::cout << "IOCHANNEL READBLE\n";
				processing = true;
				break;
			case io::WRITABLE:
				std::cout << "IOCHANNLE WRITABLE\n";
				processing = true;
				break;
			case io::ERROR:
				state = IOChannelState::ERROR;
				listener->on_error();
				break;
		}

	}

}
