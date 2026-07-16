
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "EnvBuilder.hpp"
#include "EventLoop.hpp"
#include "Context.hpp"

namespace http {

template <size_t N>
Channel::Channel(Storage<N>& storage,
    Stream s, int fd, io::Event events, CgiHandler& h)
    : AEventHandler(fd, events),
    stream_(s),
    state_(Open),
    handler_(h),
    buf(storage) {}

Channel::~Channel() {}

void Channel::on_event(io::Event event) {

    size_t w = 0;
    switch (event) {
        case io::Readable: case io::Hup:
            std::cout << "Channel Readable\n";
            w = handler_.on_readable(*this);
            buf.advance_read(w);
            break;
        case io::Writable:
            std::cout << "Channel Writable\n";
            w = handler_.on_writable(buf, *this);
            buf.advance_write(w);
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

BufferView Channel::view() const {
    return BufferView(buf.read_ptr(), buf.bytes_pending());
}

void Channel::shutdown() {
    state_ = Closed;
}

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
}

void Channel::write() {
    ssize_t n = ::write(fd(), buf.read_ptr(), buf.bytes_pending());

    if (n < 0) {
        state_ = Closing;
        return;
    }

    if (n == 0) state_ = Closing;

    buf.advance_read(n);
    buf.compact();
}

time_t CgiHandler::CgiTimeoutSeconds;

CgiHandler::CgiHandler(const ResolutionResult& res,
        const http::Request& req,
        runtime::epoll::EventLoop& p,
        Context& ctx)
    : RequestHandler(ctx),
    state_(Working),
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


size_t CgiHandler::on_readable(Channel& channel) {

    channel.read();

    BufferView reader = channel.view();

    if (channel.stream() == Channel::Stderr) {
        channel.mark_closing();
        return reader.cursor();
    }

    if (channel.state() == Channel::Error) {
        state_ = Cleanup;
        reason_ = Internal;
        channel.mark_closing();
        return reader.cursor();
    }

    if (channel.state() == Channel::Closing) {
        channel.mark_closing();
    }

    ResponseParser::ParseResult r = builder.parse(reader);
    
    if (r == ResponseParser::Continue) return reader.cursor();

    if (r == ResponseParser::ParseError) {
        std::cout << "Parse Error\n";
        reason_ = ParseError;
        response_state = Error;
        return reader.cursor();
    }

    CGIResult result = builder.result();

    setStatus(result.status_code);
    const Headers& headers = result.headers;
    Headers::const_iterator it = headers.begin();
    for (; it != headers.end(); ++it) {
        setHeader(it->name, it->value);
    }

    if (result.mem_) {
        setBodyFixed(result.body_);
    } else {
        setBodyFile(result.body_filename);
    }

    response_state = Finished;
    return reader.cursor();
}

size_t CgiHandler::on_writable(Buffer& writer, Channel& channel) {
    base::io::Reader& body = request().body;

    size_t n = body.read(writer.write_ptr(), writer.bytes_free());

    writer.advance_write(n);

    if (writer.size() == 0) {
        channel.mark_closing();
        return 0;
    }

    channel.write();
    return n;
}

http::Error CgiHandler::handle() {
    
    return ERR_NONE;
}

bool CgiHandler::finished() {
    return state_ == Done;
}

bool CgiHandler::timedout() {
    bool out = spawn_time.elapsed() > CgiTimeoutSeconds;

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

        StatusCode c = OK;
        switch (reason_) {

            case None: break;
            case Timeout: c = GATEWAY_TIMEOUT; std::cout << "Gateway timeout\n"; break;
            case ParseError: c = BAD_GATEWAY; std::cout << "Bad Gateway\n"; break;
            case ProcessError: case Internal: c = INTERNAL_SERVER_ERROR; std::cout << "Internal\n"; break;

            setStatus(c);
            setHeader("Connection", "close");
            
        }
 
    }

    if (state_ == Cleanup) state_ = Done;
}

}
