
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "CGIBodyProvider.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"
#include "ServerContext.hpp"

namespace http {

CGIRequestHandler::CGIRequestHandler(const ResolutionResult& res, http::Request const& req, ServerContext& ctx)
    : state_(Headers),
    process(cgi::resolve_exec_context(req, res)),
    stdin_ch(Channel::Stdin, process.stdin_pipe().write_end(), io::Writable, *this),
    stdout_ch(Channel::Stdout, process.stdout_pipe().read_end(), io::Readable, *this),
    stderr_ch(Channel::Stderr, process.stderr_pipe().read_end(), io::Readable, *this),
    poller_(*ctx.poller),
    protocol_(*ctx.session_) {

    if (!process.running()) {
        state_ = Error;
        return;
    }

    if (process.want_stdin())
        poller_.add(&stdin_ch);
    poller_.add(&stdout_ch);
    poller_.add(&stderr_ch);
}

CGIRequestHandler::~CGIRequestHandler() {
    poller_.del(&stdin_ch);
    poller_.del(&stdout_ch);
    poller_.del(&stderr_ch);
}

void CGIRequestHandler::handle() {}

bool CGIRequestHandler::finished() {
    return state_ == Finished;
}

void CGIRequestHandler::on_readable(BufferReader& reader, Channel::Stream s) {
    (void)s;



    if (state_ == Headers) {
        
        builder.parse_headers(reader);
        if (!builder.finished()) return;

        CGIResult result(reader);        
        protocol_.on_cgi_ready(result);

        state_ = StreamingBody;
    }

}

void CGIRequestHandler::on_writable(BufferWriter& writer, Channel::Stream s) {
    (void)s;
    // read body from the request body
    std::string body = "Hello world\n";
    writer.write(body.c_str(), body.size());
}

void CGIRequestHandler::on_ch_error(Channel::Stream s) {
    (void)s;
    state_ = Error;
}

void CGIRequestHandler::on_ch_closed(Channel::Stream s) {
    (void)s;
    state_ = Finished;
}

CGIRequestHandler::State CGIRequestHandler::state() const {
    return state_;
}

}
