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
				on_error();
				break;
		}
	}

	void IOChannel::on_writeable() {
		listener->produce_output(writebuff, SEND_CHUNK_SIZE);
		write();
	}

	void IOChannel::on_readable() {
		
		while (true) {

			if (readbuf_drained()) {
				
				read();
				
				if (bytes_r < 0)
					return ;
				bytes_received = bytes_r;
			}
			
			listener->on_input_ready(readbuf, bytes_received);
		}
		
	}

	void IOChannel::on_error() {
		listener->on_error();
	}
}
