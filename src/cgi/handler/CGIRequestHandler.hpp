#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"
#include "Channel.hpp"

namespace runtime { namespace epoll { class EventPoller; };}

namespace http {

class Context;
struct ResolutionResult;
struct Request;

struct CGIResult {
	StatusCode	code;
	std::string status_reason;
	Headers		headers;
	BufferReader& source_;

	CGIResult(BufferReader& source): code(OK), status_reason(), headers(), source_(source) {}
};



class CGIRequestHandler: public IRequestHandler {

public:
	enum State {
		Headers,
		StreamingBody,
		Error,
		Finished,
	};

private:
	State state_;
	
	cgi::Process process;
	
	ResponseParser builder;

	Channel stdin_ch;
	Channel stdout_ch;
	Channel stderr_ch;

	runtime::epoll::EventPoller& poller_;
	
	Context& ctx_;
	
	CGIRequestHandler(const CGIRequestHandler&);
	CGIRequestHandler& operator=(const CGIRequestHandler&);

public:
	CGIRequestHandler(const ResolutionResult& result, const http::Request& req,
		runtime::epoll::EventPoller& poller_, Context& ctx);
	~CGIRequestHandler();
	State state() const;
	
	void handle();
	bool done();
	
	void on_writeable(Channel& ch);
	void on_readable(Channel& ch);
	
	void on_ch_error(Channel& h);
	void ob_ch_closed(Channel& h);

	void pause_channel(Channel& ch);
	void resume_channel(Channel& ch);

};

}
