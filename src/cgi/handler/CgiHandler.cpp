
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
    body_fd(-1),
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

    if (body_fd >= 0) ::close(body_fd);
}

void CgiHandler::handle() {}

bool CgiHandler::finished() {
    return state_ == Finished;
}

CgiHandler::State CgiHandler::state() const {
    return state_;
}

CGIResult CgiHandler::result() {
    if (state_ == Error) return CGIResult(-1, "", INTERNAL_SERVER_ERROR, builder.headers());
    
    int tmp_fd = body_fd;
    body_fd = -1;

    return CGIResult(tmp_fd, body_filename, builder.status_code(), builder.headers());
}


}
