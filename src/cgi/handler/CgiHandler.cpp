
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "EventPoller.hpp"

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

    if (process.want_stdin()) poller_.add(&stdin_ch);
    poller_.add(&stdout_ch);
    poller_.add(&stderr_ch);
}

CgiHandler::~CgiHandler() {
    if (!stdin_ch.closed()) poller_.del(&stdin_ch);
    if (!stdout_ch.closed()) poller_.del(&stdout_ch);
    if (!stderr_ch.closed()) poller_.del(&stderr_ch);
}

void CgiHandler::handle() {

}

bool CgiHandler::finished() {
    return state_ == Finished;
}

CgiHandler::State CgiHandler::state() const {
    return state_;
}

CGIResult CgiHandler::result() const {
    return CGIResult(builder.status_code(), builder.headers());
}

void CgiHandler::pause_channel(Channel::Stream type) {

    Channel* ch = NULL;

    switch (type) {
        case Channel::Stdin: ch = &stdin_ch; break;
        case Channel::Stdout: ch = &stdout_ch; break;
        case Channel::Stderr: ch = &stderr_ch; break;
    }
    
    if (ch) ch->pause();
}

void CgiHandler::resume_channel(Channel::Stream type) {
    
    Channel* ch = NULL;
    
    switch (type) {
        case Channel::Stdin: ch = &stdin_ch; break;
        case Channel::Stdout: ch = &stdout_ch; break;
        case Channel::Stderr: ch = &stderr_ch; break;
    }
    
    if (ch) ch->resume();
}



}
