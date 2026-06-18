#include "CgiHandler.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"
#include <fcntl.h>
#include <cerrno>

namespace http {

void CgiHandler::on_readable(BufferReader& reader, Channel& channel) {

    channel.read();

    if (channel.stream() == Channel::Stderr) {
        channel.mark_closing();
        return;
    }

    ResponseParser::ParseResult r = builder.parse_headers(reader);
    
    if (r == ResponseParser::Continue) return;

    if (r == ResponseParser::ParseError) {
        reason_ = ParseError;
        response_state = Error;
        return;
    }

    response_state = Finished;
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
