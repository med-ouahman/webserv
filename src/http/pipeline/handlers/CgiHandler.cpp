
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "EventPoller.hpp"
#include "Context.hpp"

namespace http {

template <size_t N>
Channel::Channel(Storage<N>& storage,
    Stream s, int fd, io::Event events, CgiHandler& h)
    : AEventHandler(fd, events),
    stream_(s),
    state_(Open),
    handler_(h),
    buf(storage),
    view_(buf.read_ptr(), 0) {}

Channel::~Channel() {}

void Channel::on_event(io::Event event) {

    switch (event) {
        case io::Readable: case io::Hup:
            std::cout << "Channel Readable\n";
            handler_.on_readable(buf, *this);
            break;
        case io::Writable:
            std::cout << "Channel Writable\n";
            handler_.on_writable(buf, *this);
            break;
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

bool Channel::writable() const {
    return buf.writable();
}

BufferView& Channel::view() { return view_; }

void Channel::mark_closing() {
    if (state_ == Closed) return;
    
    state_ = Closing;
}

void Channel::read() {
    
    buf.compact();

    ssize_t n = ::read(fd(), buf.write_ptr(), buf.bytes_free());

    if (n < 0) {
        state_ = Error;
        return;
    }

    if (n == 0) state_ = Closing;
    buf.advance_write(n);

    view_.update(buf.size());
}

void Channel::write() {
    ssize_t n = ::write(fd(), buf.read_ptr(), buf.bytes_pending());

    if (n < 0) {
        state_ = Error;
        return;
    }

    state_ = Closing;

    buf.advance_read(n);
    buf.compact();
}

time_t CgiHandler::CgiTimeoutSeconds;

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
    stdin_ch(stdin_wbuf, Channel::Stdin, process.stdin_pipe().write_end(), io::Writable, *this),
    stdout_ch(stdout_rdbuf, Channel::Stdout, process.stdout_pipe().read_end(), io::Readable, *this),
    stderr_ch(stderr_rdbuf, Channel::Stderr, process.stderr_pipe().read_end(), io::Readable, *this),

    poller_(p),
    ctx_(ctx) {

    CgiTimeoutSeconds = 5; // 30 seconds is generous
    
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


CgiHandler::State CgiHandler::state() const { return state_; }

void CgiHandler::on_readable(Buffer& rdbuf, Channel& channel) {

    if (channel.stream() == Channel::Stderr) {
        channel.mark_closing();
        return;
    }

    if (channel.state() == Channel::Error) {
        state_ = Cleanup;
        reason_ = Internal;
        channel.mark_closing();
        return;
    }

    if (channel.state() == Channel::Closing) {
        channel.mark_closing();
    }

    if (response_state == Processing) {

        channel.read();
        ResponseParser::ParseResult r = builder.parse(channel.view());
        if (r == ResponseParser::Continue) return;
        
        if (r == ResponseParser::ParseError) {
            reason_ = ParseError;
            response_state = Error;
            return;
        }
        
        response_state = BodyStreaming;
        
        ctx_.on_cgi_headers(builder.result());
        
    } else if (response_state == BodyStreaming) {

        channel.read();
        
        size_t w = ctx_.on_cgi_body(channel.view());
        
        rdbuf.advance_read(w);
        
        if (!rdbuf.writable()) channel.pause();
    }
}

void CgiHandler::on_writable(Buffer& writer, Channel& channel) {

    base::Expected<usize, base::io::Error> res = ctx_.request_body().read(writer.write_ptr(), writer.bytes_free());

    if (!res.has_value()) {
        state_ = Cleanup;
        reason_ = Internal;
        return;
    }

    usize n = res.value();

    writer.advance_write(n);

    if (writer.size() == 0) {
        channel.mark_closing();
        return;
    }
    writer.advance_read(n);

    channel.write();
}

void CgiHandler::handle() {}

bool CgiHandler::finished() const {
    return state_ == Done;
}


void CgiHandler::resume() {
    stdout_ch.resume();
}

bool CgiHandler::timedout() {

    bool t = spawn_time.elapsed() > CgiTimeoutSeconds;

    if (t) reason_ = Timeout;
    
    return t;
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

    bool buffer_has_space = stdout_ch.writable();

    if (response_state == BodyStreaming && buffer_has_space) stdout_ch.resume();

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
        StatusCode status_code;
        switch (reason_) {

            case None: break;
            case Timeout: status_code = GATEWAY_TIMEOUT; std::cout << "Gateway timeout\n"; break;
            case ParseError: status_code = BAD_GATEWAY; std::cout << "Bad Gateway\n"; break;
            case ProcessError: case Internal: status_code = INTERNAL_SERVER_ERROR; std::cout << "Internal\n"; break;
        }
    
        ctx_.on_cgi_error(status_code);
    }

    if (state_ == Cleanup) state_ = Done;
}

}
