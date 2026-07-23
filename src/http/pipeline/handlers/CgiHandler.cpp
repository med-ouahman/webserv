
#include "CgiHandler.hpp"
#include "Request.hpp"

#include "runtime/epoll/EventLoop.hpp"
#include "Context.hpp"


#include <cerrno>
#include <iostream>

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
	size_t n = 0;

	if (event & io::Error) {
			state_ = Closing;
			return;
	}

	if (event & io::RHup)
			state_ = Closing;

	if (event & io::Readable || event & io::Hup) {
			n = handler_.on_readable(*this);
			buf.advance_read(n);
			return;
	}

	if (event & io::Writable) {
			handler_.on_writable(buf, *this);
			return;
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
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
	    state_ = Error;
	    return;
	}

    if (n == 0) state_ = Closing;
    buf.advance_write(n);
}

void Channel::write() {
	ssize_t n = ::write(fd(), buf.read_ptr(), buf.bytes_pending());

	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
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
	    stdin_added(false),
	    stdout_added(false),
	    stderr_added(false),

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

    timeout_seconds = request_ctx.timeout;
    if (!process.start(process_ctx)) {
        state_ = Cleanup;
        reason_ = Internal;
        return ;
    }

	    if (process.want_stdin())
	        stdin_added = event_loop.add(&stdin_ch);
	    else {
	        stdin_ch.close();
	        stdin_ch.shutdown();
	    }

	    stdout_added = event_loop.add(&stdout_ch);
	    stderr_added = event_loop.add(&stderr_ch);
}

CgiHandler::~CgiHandler() {
	if (stdin_added) event_loop.del(&stdin_ch);
	if (stdout_added) event_loop.del(&stdout_ch);
	if (stderr_added) event_loop.del(&stderr_ch);
}


size_t CgiHandler::on_readable(Channel& channel) {

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
        return reader.cursor();
    }

    response_state = Finished;
    return reader.cursor();
}

size_t CgiHandler::on_writable(Buffer& writer, Channel& channel) {

	base::io::Reader& body = request().body;
	usize n = 0;

	if (writer.bytes_pending() != 0) {
		channel.write();
		return 0;
	}

	base::Expected<usize, base::io::Error> result = body.read(writer.write_ptr(), writer.bytes_free());

	if (!result.has_value()) {
	    state_ = Cleanup;
	    reason_ = Internal;
        channel.mark_closing();
        return 0;
	}
	n = result.value();

	if (n == 0) {
		channel.mark_closing();
		return 0;
	}

	writer.advance_write(n);
	channel.write();
	return 0;
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

bool* CgiHandler::added_flag(Channel& channel) {
	if (channel.stream() == Channel::Stdin)
		return &stdin_added;
	if (channel.stream() == Channel::Stdout)
		return &stdout_added;
	return &stderr_added;
}

void CgiHandler::close_channel(Channel& ch) {
	bool* added = added_flag(ch);
	if (*added) {
		event_loop.del(&ch);
		*added = false;
	}
	ch.close();
}

void CgiHandler::check_process() {

	process.reap();
	if (process.reaped()) {
		shutdown_state = Reaped;
		return;
	}

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
	if (timedout()) {
			reason_ = Timeout;
			state_ = Cleanup;
	}

	if (response_state == Error)
			state_ = Cleanup;
	if (response_state == Finished)
			state_ = Cleanup;

	if (state_ == Cleanup && shutdown_state != Reaped)
			check_process();

	if (process.reaped())
			shutdown_state = Reaped;

	check_channels();

	if (shutdown_state != Reaped)
			return;

	cgi::ProcessResult res = process.result();

	if (res.reason != cgi::Exited && reason_ == None)
			reason_ = Internal;

	if (state_ == Cleanup)
			state_ = Done;
}

http::Error CgiHandler::handle() {

    switch (reason_) {
		case None: break;
		case Timeout: return ERR_CGI_TIMEOUT;
		case ParseError: return ERR_BAD_GATEWAY;
		case ProcessError: case Internal: return ERR_INTERNAL;
    }
    if (response_state != Finished) return ERR_NONE;

    CGIResult result = builder.result();
    const Headers& headers = result.headers;

	Headers::const_iterator it = headers.begin();

	while (it != headers.end()) {
		setHeader(it->name, it->value);
		++it;
	}

	if (result.mem_) setBodyFixed(result.body_);
	else setBodyFile(result.body_filename);

    setStatus(result.status_code);
	setConnection();
	setDate();
	responseReady();
    return ERR_NONE;
}

}
