
#include "Timestamp.hpp"
#include "CGIRequestHandler.hpp"
#include "Dispatcher.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "CGIBodyProvider.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"

namespace http {

CGIRequestHandler::CGIRequestHandler(const ResolutionResult& res, http::Request const& req, runtime::epoll::EventPoller& poller, Context& ctx)
    : state_(Headers),
    process(cgi::resolve_exec_context(req, res)),
    stdin_ch(process.stdin_pipe().write_end(), io::WRITABLE, *this),
    stdout_ch(process.stdout_pipe().read_end(), io::READABLE, *this),
    stderr_ch(process.stderr_pipe().read_end(), io::READABLE, *this),
    poller_(poller),
    ctx_(ctx) {

    if (!process.running()) {
        state_ = Error;
        return;
    }

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

bool CGIRequestHandler::done() {
    return state_ == Finished;
}

void CGIRequestHandler::on_readable(BufferReader& reader) {

    if (state_ == Headers) {
        
        builder.parse_headers(reader);
        if (!builder.finished()) return;

        CGIResult result(reader);        
        ctx_.on_cgi_ready(result);

        state_ = StreamingBody;
    }

}

void CGIRequestHandler::on_writeable(BufferWriter& w) {
    // read body from the request body
    std::string body = "Hello world\n";
    w.write(body.c_str(), body.size());
}

void CGIRequestHandler::on_ch_error() {
    state_ = Error;
}

void CGIRequestHandler::on_ch_closed() {
    state_ = Finished;
}

CGIRequestHandler::State CGIRequestHandler::state() const {
    return state_;
}

}
