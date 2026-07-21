#pragma once

#include "runtime/AEventHandler.hpp"

#include "cgi/Process.hpp"
#include "cgi/ResponseParser.hpp"
#include "Buffer.hpp"
#include "RequestHandler.hpp"
#include "cgi/CGIContext.hpp"

namespace runtime { namespace epoll { class EventLoop; } }

namespace http {

class CgiHandler;

class Channel: public io::AEventHandler {
public:
enum Stream {
	Stdin,
	Stdout,
	Stderr
};

enum State {
	Open,
	Closing,
	Closed,
	Error
};

private:
	Stream stream_;
	State state_;
	CgiHandler& handler_;
	Buffer buf;

public:
	template <size_t N>
	Channel(Storage<N>& storage,
		Stream s,
		int fd,
		io::Event events,
		CgiHandler& handler);
	~Channel();
	
	void on_event(io::Event ev);

	void read();
	void write();

	Stream stream() const;
	State state() const;
	void shutdown();
	void mark_closing();
	BufferView view() const;
};

class CgiHandler: public ARequestHandler {
public:

enum State {
	Working,
	Cleanup,
	Done,
};

enum ResponseState {
	Processing,
	Finished,
	Error,
};

enum FailureReason {
    None,
    ParseError,
    Timeout,
	ProcessError,
	Internal
};

enum ShutdownState {
	SigTerm,
	WaitingSigTerm,
	SigKill,
	Reaping,
	Reaped,
};

private:

	const static std::size_t StdinWriteSize = 4096;
	const static std::size_t StdoutReadSize = 4096;
	const static std::size_t StderrReadSize = 1024;

	const static time_t SigTermWaitSeconds = 3;

	State state_;
	ResponseState response_state;
	FailureReason reason_;
	
	cgi::Process 	process;
    Timestamp		spawn_time;
    Timestamp		sigterm_sent_at;
	
	ShutdownState	shutdown_state;
	
	cgi::CGIRequestContext request_ctx;
	ResponseParser builder;
	
	Storage<StdinWriteSize> stdin_wbuf;
	Storage<StdoutReadSize> stdout_rdbuf;
	Storage<StderrReadSize> stderr_rdbuf;
	
	Channel stdin_ch;
	Channel stdout_ch;
	Channel stderr_ch;
	
	runtime::epoll::EventLoop& event_loop;
	time_t timeout_seconds;
	
	CgiHandler(const CgiHandler&);
	CgiHandler& operator=(const CgiHandler&);

	bool timedout();
	void check_process();
	void check_channels();
	
public:
	CgiHandler(Context& ctx);

	~CgiHandler();

	bool done() const; // FOR RESPONSE, INHERITED FROM AREQUEST_HANDLER

	bool tobenamedlater(); // INDICATES RESOURCES ARE CLEANED AND SAFE TO delete;

	size_t on_writable(Buffer& writer, Channel& channel);
	size_t on_readable(Channel& channel);	
	void close_channel(Channel& channel);

 	http::Error handle();
	
	void monitor();
};

}
