
#include "CgiHandler.hpp"
#include "Request.hpp"

#include "runtime/epoll/EventLoop.hpp"
#include "Context.hpp"


#include <iostream>

void log_cgi_request_context(const cgi::CGIRequestContext& ctx)
{
    std::cout
        << "===== CGI Request Context =====\n"
        << "REQUEST_METHOD : " << ctx.request_method   << '\n'
        << "MIME_TYPE      : " << ctx.mime_type        << '\n'
        << "INTERPRETER    : " << ctx.interpreter      << '\n'
        << "SCRIPT_NAME    : " << ctx.script_name      << '\n'
        << "QUERY_STRING   : " << ctx.query_string     << '\n'
        << "CONTENT_LENGTH : " << ctx.content_length   << '\n'
        << "PATH_INFO      : " << ctx.path_info        << '\n'
        << "SERVER_NAME    : " << ctx.server_name      << '\n'
        << "SERVER_PROTOCOL: " << ctx.server_protocol  << '\n'
        << "SERVER_PORT    : " << ctx.server_port      << '\n'
        << "TIMEOUT        : " << ctx.timeout << " s\n"
        << "===============================\n";
}

void log_process_context(const cgi::ProcessContext& ctx)
{
    std::cout
        << "======== Process Context ========\n"
        << "WORKING_DIR : " << ctx.working_dir << '\n'
        << "STDIN_FD    : " << ctx.stdin_fd.get() << '\n';

    std::cout << "ARGV (" << ctx.argv.size() << ")\n";
    for (std::size_t i = 0; i < ctx.argv.size(); ++i)
    {
        std::cout << "  [" << i << "] " << ctx.argv.data()[i] << '\n';
    }

    std::cout << "ENVP (" << ctx.envp.size() << ")\n";
    for (std::size_t i = 0; i < ctx.envp.size(); ++i)
    {
        std::cout << "  [" << i << "] " << ctx.envp.data()[i] << '\n';
    }

    std::cout
        << "=================================\n";
}

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

CgiHandler::CgiHandler(Context& ctx)
    : ARequestHandler(ctx),
    state_(Working),
    response_state(Processing),
    reason_(None),
    spawn_time(),
    sigterm_sent_at(0),
    shutdown_state(SigTerm),
    stdin_ch(stdin_wbuf, Channel::Stdin, process.stdin_pipe().write_end(), io::Writable, *this),
    stdout_ch(stdout_rdbuf, Channel::Stdout, process.stdout_pipe().read_end(), io::Readable, *this),
    stderr_ch(stderr_rdbuf, Channel::Stderr, process.stderr_pipe().read_end(), io::Readable, *this),

    event_loop(ctx.services_.poller) {
    
    if (!process.running() || state_ != Working) {
        state_ = Cleanup;
        reason_ = Internal;
        return;
    }

    cgi::CGIRequestContext request_ctx;
    cgi::ProcessContext process_ctx;


    http::Error r = cgi::buildCGIContext(ctx, request_ctx, process_ctx);

    if (r != ERR_NONE) {
        state_ = Cleanup;
        reason_ = Internal;
        return ;
    }

    log_cgi_request_context(request_ctx);

    log_process_context(process_ctx);

    timeout_seconds = request_ctx.timeout;
    if (!process.start(process_ctx)) {
        state_ = Cleanup;
        reason_ = Internal;
        return;
    }

    if (process.want_stdin()) event_loop.add(&stdin_ch);
    else stdin_ch.shutdown();

    event_loop.add(&stdout_ch);
    event_loop.add(&stderr_ch);
}

CgiHandler::~CgiHandler() {
    if (stdin_ch.state() != Channel::Closed) event_loop.del(&stdin_ch);
    if (stdout_ch.state() != Channel::Closed) event_loop.del(&stdout_ch);
    if (stderr_ch.state() != Channel::Closed) event_loop.del(&stderr_ch);
}


size_t CgiHandler::on_readable(Channel& channel) {

    std::cerr << "Nigga\n";
    channel.read();

    if (channel.state() == Channel::Closing) {
        channel.mark_closing();
    }

    BufferView reader = channel.view();

    if (channel.state() == Channel::Error) {
        state_ = Cleanup;
        reason_ = Internal;
        channel.mark_closing();
        return reader.cursor();
    }

    if (channel.stream() == Channel::Stderr) {
        std::cerr.write(reader.data(), reader.remaining());
        reader.advance(reader.remaining());
        return reader.cursor();
    }


    ResponseParser::ParseResult r = builder.parse(reader);
    
    if (r == ResponseParser::Continue) return reader.cursor();

    if (r == ResponseParser::ParseError) {
        reason_ = ParseError;
        response_state = Error;
        std::cout << "Error\n";
        return reader.cursor();
    }

    std::cout << "good to go\n";

    response_state = Finished;
    return reader.cursor();
}

size_t CgiHandler::on_writable(Buffer& writer, Channel& channel) {

    base::io::Reader& body = request().body;
    usize n = 0;

    base::Expected<usize, base::io::Error> result = body.read(writer.write_ptr(), writer.bytes_free());
    
    if (result.has_value()) {
        n = result.value();
        std::cout << "n: " << n <<  " body[0]: " << int(writer.read_ptr()[0]) << "\n";
    }

    writer.advance_write(n);
    
    if (writer.size() == 0) {
        channel.mark_closing();
        return 0;
    }
    
    channel.write();

    return n;
}

http::Error CgiHandler::handle() {

    if (response_state == Finished) {
        CGIResult result = builder.result();
        setStatus(result.status_code);

        const Headers& headers = result.headers;
        Headers::const_iterator it = headers.begin();

        for ( ;it != headers.end(); ++it) {
            setHeader(it->name, it->value);
        }
    
        if (result.mem_) {
            setBodyFixed(result.body_);
        } else {
            setBodyFile(result.body_filename);
        }
    }

    if (reason_ == None) return ERR_NONE;

    switch (reason_) {
        case None: break;
        case Timeout: return ERR_CGI_TIMEOUT;
        case ParseError: return ERR_BAD_GATEWAY;
        case ProcessError: case Internal: return ERR_INTERNAL;
    }

    return ERR_NONE;
}

bool CgiHandler::done() const {
    return response_state == Finished || response_state == Error;
}

bool CgiHandler::can_close() const { return state_ == Done; }

bool CgiHandler::timedout() {
    bool out = spawn_time.elapsed() > timeout_seconds;

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
    event_loop.del(&ch);
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

    std::cout << "MONITORING THE CGI HANDLER IN \n";
    if ((response_state == Finished
        || response_state == Error) || timedout()) state_ = Cleanup;

    if (state_ == Cleanup
        && shutdown_state != Reaped) check_process();

    if (process.reaped()) shutdown_state = Reaped;

    check_channels();

    if (shutdown_state != Reaped) return;

    cgi::ProcessResult res = process.result();

    if (res.reason != cgi::Exited && reason_ == None) reason_ = Internal;

    if (state_ == Cleanup) state_ = Done;
}

}
