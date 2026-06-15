#include "CgiHandler.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"
#include <fcntl.h>

namespace http {

void CgiHandler::on_readable(BufferReader& reader, Channel& channel) {

    channel.read();

    if (reader.size() == 0) {
        protocol_.on_cgi_ready();
        state_ = Finished;
    }

    std::cout.write(reader.data(), reader.size());

    if (state_ == Headers) {
        
        ParseResult r = builder.parse_headers(reader);
        
        if (r == ParseError || !builder.finished()) {
            state_ = r == ParseError ? Error: state_;
            return;
        }

        if (body_fd < 0) {

            body_filename = "/tmp/" + base::random_string(10);
            body_fd = ::open(body_filename.c_str(), O_TMPFILE, 0600);

            if (body_fd < 0) {
                state_ = Error;
                return;
            }
            state_ = StreamingBody;
        }
        
    }

    if (state_ == StreamingBody) {
        ssize_t w = ::write(body_fd, reader.data() + reader.cursor(), reader.size());

        if (w < 0) {
            state_ = Error;
            return;
        }

        reader.advance(w);
    }
}

void CgiHandler::on_writable(BufferWriter& writer, Channel& channel) {
    // read body from the request body
    std::string body = "Hello world\n";
    writer.write(body.c_str(), body.size());
    channel.write();
}

void CgiHandler::on_ch_error(Channel& channel) {
    state_ = Error;
    close_channel(channel);
}

void CgiHandler::on_ch_closed(Channel& channel) {
    std::cout << "Closed\n";
    close_channel(channel);
}

void CgiHandler::close_channel(Channel& channel) {
    if (channel.closed()) return;

    poller_.del(&channel);
}

}
