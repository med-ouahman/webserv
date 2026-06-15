#include "CgiHandler.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"

namespace http {

void CgiHandler::on_readable(BufferReader& reader, Channel& channel) {
   
    if (state_ == Error) return;

    if (state_ == Headers || reader.empty()) channel.read();
    else {
        channel.pause();
        return;
    }
    
    std::cout.write(reader.data(), reader.size());
    
    if (state_ == Headers) {
        
        ParseResult r = builder.parse_headers(reader);
        
        if (r == ParseError || !builder.finished()) {
            state_ = r == ParseError ? Error: state_;
            return;
        }
    }

    protocol_.on_cgi_ready(reader);

    if (state_ == Headers) state_ = StreamingBody;
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
