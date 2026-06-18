#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"
#include "Channel.hpp"

namespace runtime { namespace epoll { class EventPoller; } }

namespace http {

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
	static time_t cgi_timeout_sec;

	State state_;

	ResponseState response_state;
	FailureReason reason_;

	cgi::Process 	process;
    Timestamp		spawn_time;
    Timestamp		sigterm_sent_at;

	ShutdownState	shutdown_state;

	int 		body_fd;
	std::string body_filename;

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
	void on_readable(BufferReader& reader, Channel& channel);
	
	void close_channel(Channel& channel);
	void refresh_state();
};

}
