
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"

namespace http {

time_t CgiHandler::cgi_timeout_sec;

CgiHandler::CgiHandler(const ResolutionResult& res,
        const http::Request& req,
        runtime::epoll::EventPoller& p,
        Context& ctx)
    : state_(Working),
    response_state(Headers),
    reason_(None),
    process(cgi::resolve_exec_context(req, res)),
    spawn_time(),
    sigterm_sent_at(0),
    body_fd(-1),
    stdin_ch(Channel::Stdin, process.stdin_pipe().write_end(), io::Writable, *this),
    stdout_ch(Channel::Stdout, process.stdout_pipe().read_end(), io::Readable, *this),
    stderr_ch(Channel::Stderr, process.stderr_pipe().read_end(), io::Readable, *this),
    poller_(p),
    protocol_(ctx) {
    cgi_timeout_sec = 5; // 30 seonds is generous
    
    if (!process.running()) {
        state_ = Cleanup;
        reason_ = Internal;
        return;
    }

    if (process.want_stdin()) poller_.add(&stdin_ch);
    else stdin_ch.shutdown();

    poller_.add(&stdout_ch);
    poller_.add(&stderr_ch);
}

CgiHandler::~CgiHandler() {
    
    if (stdin_ch.state() != Channel::Closed) poller_.del(&stdin_ch);
    if (stdout_ch.state() != Channel::Closed) poller_.del(&stdout_ch);
    if (stderr_ch.state() != Channel::Closed) poller_.del(&stderr_ch);

    if (body_fd >= 0) ::close(body_fd);
    body_fd = -1;
}

void CgiHandler::handle() {}

bool CgiHandler::finished() {
    return state_ == Done;
}

CgiHandler::FailureReason CgiHandler::reason() const { return reason_; }

void CgiHandler::check_timeout() {
    if (spawn_time.elapsed() > cgi_timeout_sec) reason_ = Timeout;
}

void CgiHandler::check_channels() {
    
    Channel* channels[] = { &stdin_ch, &stdout_ch, &stderr_ch };

    for (size_t i = 0; i < sizeof(channels) / sizeof(channels[0]); ++i) {
        Channel& ch = *channels[i];

        if (state_ == Cleanup) ch.mark_closing();

        if (ch.state() == Channel::Closing) {
            close_channel(ch);
            ch.shutdown();
        }
    }

}

void CgiHandler::check_process() {

    process.poll();

    if (process.reaped()) {
        shutdown_state = Reaped;
        return;
    }

    if (reason_ == None) check_timeout();

    switch (shutdown_state) {
        case SigTerm:
            sigterm_sent_at.update();
            process.terminate();
            shutdown_state = WaitingSigTerm;
            break;
        case WaitingSigTerm:
            if (sigterm_sent_at.elapsed() > 2) shutdown_state = SigKill;
            break;
        case SigKill:
            process.kill();
            shutdown_state = Reaping;
            break;
        case Reaping:
            break;
        case Reaped:
            break;
    }
}

void CgiHandler::refresh_state() {

    if (response_state == Finished || response_state == Error) state_ = Cleanup;

    if (shutdown_state != Reaped) check_process();

    check_channels();

    if (shutdown_state != Reaped) return;

    if (reason_ != None) {
        CGIResult result;
        
        switch (reason_) {
            case Timeout: result.status_code = GATEWAY_TIMEOUT; break;
            case ProcessError: case ParseError: result.status_code = BAD_GATEWAY; break;
            case Internal: result.status_code = INTERNAL_SERVER_ERROR; break;
            
            default: break;
        }
    
        protocol_.on_cgi_ready(result);
    }

    state_ = Done;
}

}
