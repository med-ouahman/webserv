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
	Headers,
	HeadersDone,
	StreamingBody,
	Finished,
	Error,
};

private:
	State state_;
	
	cgi::Process process;
	
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

public:
	CgiHandler(const ResolutionResult& result,
		const http::Request& req,
		runtime::epoll::EventPoller& p,
		Context& ctx);

	~CgiHandler();
	State state() const;
	
	void handle();
	bool finished();
	
	void on_writable(BufferWriter& writer, Channel& channel);
	void on_readable(BufferReader& reader, Channel& channel);
	
	void close_channel(Channel& channel);
	void sync();
};

}
