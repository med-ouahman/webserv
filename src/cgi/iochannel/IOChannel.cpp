
#include "IOChannel.hpp"
#include "CGIRequestHandler.hpp"

namespace http {

    IOChannel::IOChannel( int fd_, CGIRequestHandler& h, STDStream::Type stream_type, uint32_t event_mask )
        : Stream(fd_),
        cgi_handler(h),
        stream(stream_type),
        event(event_mask),
        state(IOChannelState::IDLE) {}

    IOChannel::~IOChannel() {}

    uint32_t IOChannel::get_event() const {
        return event;
    }

    bool IOChannel::readbuf_drained() {
        return data_view.empty();
    }

    DataView& IOChannel::get_view( ) {
        return data_view;
    }

    void IOChannel::shutdown() {
        state = IOChannelState::CLOSED;
    }

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

		if (state == IOChannelState::CLOSED) {
			std::cout << "CLOSED\n";
			return ;
		}
		
		state = IOChannelState::ACTIVE;
		switch (io_event) {
			case io::NONE:
				state = IOChannelState::IDLE;
				break;
			case io::READABLE:
				std::cout <<  c <<  " IOCHANNEL READBLE\n";
				break;
			case io::WRITABLE:
				std::cout << c << " IOCHANNLE WRITABLE\n";
				break;
			case io::HUP:
				shutdown();
				break;
			case io::ERROR:
				std::cout << c << "\n";
				state = IOChannelState::ERROR;
				cgi_handler.on_ch_error();
				break;
		}

	}
    
    void IOChannel::process() {

        switch (stream) {
            case STDStream::STDERR: case STDStream::STDOUT:
                on_readable();
                break;
            case STDStream::STDIN:
                on_writeable();
                break;
            default:
                break;
        }
    }
    
    void IOChannel::process_incoming_data() {
        
        ScanResult r = cgi_handler.on_input_ready();
        
        if (ERROR == r || SUCCESS == r) {
            state = IOChannelState::CLOSED;
        }
    }

    void IOChannel::process_outgoing_data() {
        
        if (writer.offset() < writer.size())
            return ;

        ssize_t produced = cgi_handler.produce_output(&writer);
        
        if (produced < 0 || produced == 0) {
            return ;
        }

        writer.update(produced);
    }

    void IOChannel::on_read_eof() {
        
        cgi_handler.on_channel_closed();
    }

    void IOChannel::on_write_complete() {
        shutdown();
    }

    void IOChannel::on_write_error() {
        cgi_handler.on_ch_error();
    }

    void IOChannel::on_read_error() {
        cgi_handler.on_ch_error();
    }

}
