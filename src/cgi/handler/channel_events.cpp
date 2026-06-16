#include "CgiHandler.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"
#include <fcntl.h>
#include <cerrno>

namespace http {

void CgiHandler::on_readable(BufferReader& reader, Channel& channel) {
    
    if (channel.state() == Channel::Closed) return;

    channel.read();

    if (channel.stream() == Channel::Stderr) {
        channel.mark_closing();
        return;
    }

    // std::cout.write(reader.data(), reader.size());
    builder.parse_headers(reader);

    if (!builder.finished()) return;
    
    state_ = Finished;
    protocol_.on_cgi_ready(builder.result());

}

void CgiHandler::on_writable(BufferWriter& writer, Channel& channel) {
    std::string body = "Hello world\n";
    writer.write(body.c_str(), body.size());
    channel.write();
}

void CgiHandler::close_channel(Channel& channel) {
    poller_.del(&channel);
}

}
