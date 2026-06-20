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

    ResponseParser::ParseResult r = builder.parse(reader);
    
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
    static std::string body = "Hello NIGGAS\n";
    
    if (body.size() == 0) {
        channel.mark_closing();
        return;
    }

    writer.write(body.c_str(), body.size());
    body.clear();

    channel.write();
}

void CgiHandler::close_channel(Channel& channel) {
    poller_.del(&channel);
    channel.close();
}

}
