
#include "Timestamp.hpp"
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "CGIBodyProvider.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"
#include "ServerContext.hpp"

namespace http {

CgiHandler::CgiHandler(const ResolutionResult& res,
        const http::Request& req,
        runtime::epoll::EventPoller& p,
        Context& ctx)

    : state_(Headers),
    process(cgi::resolve_exec_context(req, res)),
    stdin_ch(Channel::Stdin, process.stdin_pipe().write_end(), io::Writable, *this),
    stdout_ch(Channel::Stdout, process.stdout_pipe().read_end(), io::Readable, *this),
    stderr_ch(Channel::Stderr, process.stderr_pipe().read_end(), io::Readable, *this),
    poller_(p),
    protocol_(ctx) {

    if (!process.running()) {
        state_ = Error;
        return;
    }

    if (process.want_stdin())
        poller_.add(&stdin_ch);
    poller_.add(&stdout_ch);
    poller_.add(&stderr_ch);
}

CgiHandler::~CgiHandler() {
    
    if (!stdin_ch.closed()) poller_.del(&stdin_ch);
    if (!stdout_ch.closed()) poller_.del(&stdout_ch);
    if (!stderr_ch.closed()) poller_.del(&stderr_ch);
}

void CgiHandler::handle() {}

bool CgiHandler::finished() {
    return state_ == Finished;
}

void CgiHandler::on_readable(BufferReader& reader, Channel::Stream s) {
    (void)s;

    if (state_ == Headers) {
        
        ParseResult r = builder.parse_headers(reader);
        
        if (r == ParseError || builder.finished()) {
            state_ = r == ParseError ? Error: StreamingBody;
            CGIResult result(reader, builder.status_code(), builder.headers());
            protocol_.on_cgi_ready(result);
        }
    }

}

void CgiHandler::on_writable(BufferWriter& writer, Channel::Stream s) {
    (void)s;
    // read body from the request body
    std::string body = "Hello world\n";
    writer.write(body.c_str(), body.size());
}

void CgiHandler::on_ch_error(Channel::Stream s) {
    state_ = Error;
    close_channel(s);
}

void CgiHandler::on_ch_closed(Channel::Stream s) {
    close_channel(s);
}

void CgiHandler::close_channel(Channel::Stream type) {
    
    io::AEventHandler* handler = NULL;

    switch (type) {
        case Channel::Stdin:
            handler = &stdin_ch;
            break;
        case Channel::Stdout:
            handler = &stdout_ch;
            break;
        case Channel::Stderr:
            handler = &stderr_ch;
            break;
        default:
            handler = NULL; //  I wis I could use nullptr because these all uppercase C macros are making me want ot vomit
    }

    if (handler) {
        poller_.del(handler);
    }
}

CgiHandler::State CgiHandler::state() const {
    return state_;
}

}
