#pragma once
#include <fcntl.h>
#include "runtime/AEventHandler.hpp"

#include "cgi/Channel.hpp"
#include "cgi/Process.hpp"
#include "cgi/ResponseParser.hpp"
#include "cgi/CGIContext.hpp"

#include "ARequestHandler.hpp"

namespace runtime { namespace epoll { class EventLoop; } }

namespace http {

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
	bool started_;
	
	cgi::Process 	process;
    Timestamp		spawn_time;
    Timestamp		sigterm_sent_at;
	
	ShutdownState	shutdown_state;
	
	ResponseParser builder;
	
	Storage<StdinWriteSize> stdin_wbuf;
	Storage<StdoutReadSize> stdout_rdbuf;
	Storage<StderrReadSize> stderr_rdbuf;
	
	cgi::Channel stdin_ch;
	cgi::Channel stdout_ch;
	cgi::Channel stderr_ch;
	
	std::vector<cgi::Channel*> channels;

	runtime::epoll::EventLoop& event_loop;
	time_t timeout_seconds;
	
	CgiHandler(const CgiHandler&);
	CgiHandler& operator=(const CgiHandler&);

	bool timedout();
	http::Error start();
	void check_process();
	void check_channels();
	
public:
	CgiHandler(Context& ctx);

	~CgiHandler();

	bool done() const;

	bool can_close() const;

	size_t on_writable(Buffer& writer, cgi::Channel& channel);
	size_t on_readable(cgi::Channel& channel);
	void close_channel(cgi::Channel& channel);

 	http::Error handle();
	
	void monitor();

};

}
