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
		
		CGIState::Type s = static_cast<CGIHandler*>(listener)->get_cgi_state();
		if (s == CGIState::ERROR || s == CGIState::FINISHED) {
			state = IOChannelState::ERROR;
			std::cout << "ERRROR CGI\n";
			return ;
		}
		std::cout << "BYPASSED\n";
		state = IOChannelState::ACTIVE;
		switch (io_event) {
			case io::NONE:
				state = IOChannelState::IDLE;
				break;
			case io::READABLE:
				processing = true;
				break;
			case io::WRITABLE:
				processing = true;
				break;
			case io::ERROR:
				state = IOChannelState::ERROR;
				listener->on_error();
				break;
		}

	}

}
