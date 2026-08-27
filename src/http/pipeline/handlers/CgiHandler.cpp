
#include "CgiHandler.hpp"
#include "http/Request/Request.hpp"

#include "runtime/epoll/EventLoop.hpp"
#include "Context.hpp"
#include <cstdio>

namespace cgi {

template <size_t N>
Channel::Channel(Storage<N>& storage,
Stream s, int fd, io::Event events, http::CgiHandler& h)
    : AEventHandler(fd, events),
    stream_(s),
    state_(Open),
    handler_(h),
    buf(storage) {}

}

namespace http {

CgiHandler::CgiHandler(Context& ctx)
    : ARequestHandler(ctx),
    state_(Working),
    response_state(Processing),
    reason_(None),
    started_(false),
    process(),
    spawn_time(),
    sigterm_sent_at(0),
    shutdown_state(SigTerm),
    stdin_ch(stdin_wbuf, cgi::Channel::Stdin, process.stdin_pipe().release_write_end(), io::Writable, *this),
    stdout_ch(stdout_rdbuf, cgi::Channel::Stdout, process.stdout_pipe().release_read_end(), io::Readable, *this),
    stderr_ch(stderr_rdbuf, cgi::Channel::Stderr, process.stderr_pipe().release_read_end(),io::Readable, *this),
    event_loop(ctx.services_.poller),
    timeout_seconds(0) {
        
    if (process.error() || state_ != Working) {
        state_ = Cleanup;
        reason_ = Internal;
        return;
    }
}

http::Error CgiHandler::start() {
    if (started_) return ERR_NONE;

    cgi::CGIRequestContext request_ctx;
    cgi::ProcessContext process_ctx;

    http::Error r = cgi::buildCGIContext(context_, request_ctx, process_ctx);

    timeout_seconds = request_ctx.timeout;

    if (r != ERR_NONE) {
        state_ = Cleanup;
        reason_ = Internal;
        return ERR_INTERNAL;
    }
    
    spawn_time.update();
    if (!process.start(process_ctx)) {
        state_ = Cleanup;
        reason_ = Internal;
        return ERR_INTERNAL;
    }

    started_ = true;

    if (process.want_stdin()) {
        if (!event_loop.add(&stdin_ch)) {
            state_ = Cleanup;
            reason_ = Internal;
            return ERR_INTERNAL;
        }
        channels.push_back(&stdin_ch);
    } else {
        stdin_ch.close();
        stdin_ch.shutdown();
    }

    if (!event_loop.add(&stdout_ch)) {
        state_ = Cleanup;
        reason_ = Internal;
        return ERR_INTERNAL;
    }
    channels.push_back(&stdout_ch);

    if (!event_loop.add(&stderr_ch)) {
        state_ = Cleanup;
        reason_ = Internal;
        return ERR_INTERNAL;
    }
    channels.push_back(&stderr_ch);
    
    return ERR_NONE;
}

CgiHandler::~CgiHandler() {
    for ( size_t i(0); i < channels.size(); ++i ) {
        cgi::Channel* ch = channels[i];
        if (ch->state() != cgi::Channel::Closed) {
            close_channel(*ch);
        }
        
    }
}

size_t CgiHandler::on_readable(cgi::Channel& channel) {

    channel.read();
    BufferView view = channel.view();

    if (channel.state() == cgi::Channel::Error) {
        state_ = Cleanup;
        reason_ = Internal;
        close_channel(channel);
        return view.cursor();
    }

    /* Log CGI diagnosis errors */
    if (channel.stream() == cgi::Channel::Stderr) {
        context_.services_.logger.log_cstr(logger::Error, view.data(),
        view.remaining());

        view.advance(view.remaining());

		if (channel.state() == cgi::Channel::Closing) close_channel(channel);
        return view.cursor();
    }

    ResponseParser::ParseResult r = builder.parse(view);

	if (r == ResponseParser::Continue
		&& channel.state() == cgi::Channel::Closing)
		r = builder.finish();

    if (r == ResponseParser::Continue) return view.cursor();

    if (r == ResponseParser::ParseError) {
        reason_ = ParseError;
        response_state = Error;
		close_channel(channel);
        return view.cursor();
    }
    
    close_channel(channel);
    response_state = Finished;
    return view.cursor();
}

size_t CgiHandler::on_writable(Buffer& writer, cgi::Channel& channel) {

    base::io::Reader& body = request().body;

    base::Expected<usize, base::io::Error> result = body.read(writer.write_ptr(), writer.bytes_free());
    
    if (result.error()) {
        state_ = Cleanup;
        reason_ = Internal;
        return 0;
    }

    usize n = result.value();
    writer.advance_write(n);
    
    if (writer.size() == 0) {
        close_channel(channel);
        return 0;
    }

    channel.write();
    return n;
}

http::Error CgiHandler::handle() {

    if (!started_) {
        if (request().has_body
            && request().body.type() == base::io::Reader::NONE) {
            context_.action_ = AC_READ;
            return ERR_NONE;
        }
        return start();
    }

    if (response_state == Finished) {

        CGIResult result = builder.result();
        setStatus(result.status_code);

        const Headers& headers = result.headers;
        Headers::const_iterator it = headers.begin();

        for ( ;it != headers.end(); ++it ) setHeader(it->name, it->value);
    
        setConnection();
        setDate();

		if (result.mem_)
			setBodyFixed(result.body_);
		else {
			http::Error err = setBodyFile(result.body_filename);
			std::remove(result.body_filename.c_str());
			if (err != ERR_NONE)
				return err;
		}
        setContentLength(result.mem_
            ? result.body_.size() : result.body_content_length);
        responseReady();
    }

	if (reason_ == None || state_ != Done) return ERR_NONE;

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
    
	if (!started_ || state_ != Working || reason_ != None
		|| timeout_seconds == 0)
		return false;

    bool out = spawn_time.elapsed() > timeout_seconds;

    if (out) reason_ = Timeout;
    return out;
}

void CgiHandler::check_channels() {

    for (size_t i = 0; i < channels.size(); ++i) {
        cgi::Channel& ch = *channels[i];
		if (ch.state() == cgi::Channel::Error) {
			if (reason_ == None)
				reason_ = Internal;
			state_ = Cleanup;
		}
		if (state_ == Cleanup || ch.state() == cgi::Channel::Closing)
			close_channel(ch);
    }
}

void CgiHandler::close_channel(cgi::Channel& ch)
{
    if (ch.closed()) return;

    event_loop.del(&ch);
    ch.close();
    ch.shutdown();
}

void CgiHandler::check_process() {

    process.reap();
	if (process.reaped()) shutdown_state = Reaped;

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

    if (!started_) return;

    if ((response_state == Finished
        || response_state == Error) || timedout()) state_ = Cleanup;

    if (state_ == Cleanup
        && shutdown_state != Reaped) check_process();

    check_channels();

    if (shutdown_state != Reaped) return;
    
    cgi::ProcessResult res = process.result();
    if (res.reason != cgi::Exited && reason_ == None) reason_ = Internal;

    if (state_ == Cleanup) state_ = Done;
}

}
