#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"
#include "AEventHandler.hpp"

namespace runtime { namespace epoll { class EventPoller; } }


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

	BufferView reader_;
	BufferWriter writer_;

public:
	Channel(Stream s, int fd, io::Event events, CgiHandler& handler);
	~Channel();
	
	void on_event(io::Event ev);

	void read();
	void write();

	Stream stream() const;
	State state() const;
	void shutdown();
	void mark_closing();
};

class Context;
struct ResolutionResult;
struct Request;

class CgiHandler: public IRequestHandler {
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
	const static time_t SigTermWaitSeconds = 3;
	static time_t CgiTimeoutSeconds;

	State state_;

	ResponseState response_state;
	FailureReason reason_;

	cgi::Process 	process;
    Timestamp		spawn_time;
    Timestamp		sigterm_sent_at;

	ShutdownState	shutdown_state;
	
	ResponseParser builder;

	Channel stdin_ch;
	Channel stdout_ch;
	Channel stderr_ch;

	runtime::epoll::EventPoller& poller_;
	Context& protocol_;
	
	CgiHandler(const CgiHandler&);
	CgiHandler& operator=(const CgiHandler&);

	bool timedout();
	void check_process();
	void check_channels();
	
public:
	CgiHandler(const ResolutionResult& result,
		const http::Request& req,
		runtime::epoll::EventPoller& p,
		Context& ctx);

	~CgiHandler();

	void handle();
	bool finished();
	
	void on_writable(BufferWriter& writer, Channel& channel);
	void on_readable(BufferView& reader, Channel& channel);
	
	void close_channel(Channel& channel);

	void monitor();
};

}
