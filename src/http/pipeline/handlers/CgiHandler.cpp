
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"

namespace http {

Channel::Channel(Stream s, int fd, io::Event events, CgiHandler& h)
  : AEventHandler(fd, events),
    stream_(s),
    state_(Open),
    handler_(h) {}

Channel::~Channel() {}

void Channel::on_event(io::Event event) {

    switch (event) {
        case io::Readable: case io::Hup:
            std::cout << "Channel Readable\n";
            handler_.on_readable(reader_, *this); break;
        case io::Writable:
            std::cout << "Channel Writable\n";
            handler_.on_writable(writer_, *this); break;
        case io::RHup:
            std::cout << "Channel ReadEnd hangup\n";
            state_ = Closing; break;
        case io::Error:
            std::cout << "Channel Error\n";
            state_ = Closing; break;
        default: break;
    }
    
}

Channel::Stream Channel::stream() const { return stream_; }

Channel::State Channel::state() const { return state_; }

void Channel::shutdown() {
    state_ = Closed;
}

void Channel::mark_closing() {
    if (state_ == Closed) return;
    
    state_ = Closing;
}

void Channel::read() {
    
    reader_.compact();

    ssize_t n = ::read(fd(), reader_.write_ptr(), reader_.capacity() - reader_.cursor());

    if (n < 0) {
        state_ = Closing;
        return;
    }

    if (n == 0) state_ = Closing;
    
    reader_.update(n);
}

void Channel::write() {
    ssize_t n = ::write(fd(), writer_.read_ptr(), writer_.bytes_pending());

    if (n < 0) {
        state_ = Closing;
        return;
    }

    writer_.advance_read(n);
    writer_.compact();
}



time_t CgiHandler::cgi_timeout_sec;

CgiHandler::CgiHandler(const ResolutionResult& res,
        const http::Request& req,
        runtime::epoll::EventPoller& p,
        Context& ctx)
    : state_(Working),
    response_state(Processing),
    reason_(None),
    process(cgi::resolve(req, res)),
    spawn_time(),
    sigterm_sent_at(0),
    shutdown_state(SigTerm),
    stdin_ch(Channel::Stdin, process.stdin_pipe().write_end(), io::Writable, *this),
    stdout_ch(Channel::Stdout, process.stdout_pipe().read_end(), io::Readable, *this),
    stderr_ch(Channel::Stderr, process.stderr_pipe().read_end(), io::Readable, *this),
    poller_(p),
    protocol_(ctx) {
    cgi_timeout_sec = 5; // 30 seconds is generous
    
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
}


void CgiHandler::on_readable(BufferView& reader, Channel& channel) {

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
    static std::string body = "Hello\n";
    
    if (body.size() == 0) {
        channel.mark_closing();
        return;
    }

    writer.write(body.c_str(), body.size());
    body.clear();

    channel.write();
}



void CgiHandler::handle() {}

bool CgiHandler::finished() {
    return state_ == Done;
}

bool CgiHandler::timedout() {
    bool out = spawn_time.elapsed() > cgi_timeout_sec;

    if (out) reason_ = Timeout;
    return out;
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

void CgiHandler::close_channel(Channel& ch) {
    poller_.del(&ch);
    ch.close();
}

void CgiHandler::check_process() {

    process.reap();

    switch (shutdown_state) {
        case SigTerm:
            sigterm_sent_at.update();
            process.terminate();
            shutdown_state = WaitingSigTerm;
            break;
        case WaitingSigTerm:
            if (sigterm_sent_at.elapsed() > SigTermWaitSeconds) shutdown_state = SigKill;
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

void CgiHandler::monitor() {

    if ((response_state == Finished
        || response_state == Error) || timedout()) state_ = Cleanup;

    if (state_ == Cleanup
        && shutdown_state != Reaped) check_process();

    if (process.reaped()) shutdown_state = Reaped;

    check_channels();

    if (shutdown_state != Reaped) return;

    cgi::ProcessResult res = process.result();

    if (res.reason != cgi::Exited && reason_ == None) reason_ = Internal;

    if (reason_ != None && response_state != Finished) {
        CGIResult result;
    
        switch (reason_) {

            case None: break;
            case Timeout: result.status_code = GATEWAY_TIMEOUT; std::cout << "Gateway timeout\n"; break;
            case ParseError: result.status_code = BAD_GATEWAY; std::cout << "Bad Gateway\n"; break;
            case ProcessError: case Internal: result.status_code = INTERNAL_SERVER_ERROR; std::cout << "Internal\n"; break;
        }
    
        protocol_.on_cgi_ready(result);
    }

    if (state_ == Cleanup) state_ = Done;
}

}
