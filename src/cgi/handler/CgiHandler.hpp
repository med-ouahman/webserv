#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"
#include "Channel.hpp"

struct ServerContext;
namespace runtime { namespace epoll { class EventPoller; };}

namespace http {

class Context;
struct ResolutionResult;
struct Request;

struct CGIResult {

	StatusCode	status_code;	
	const Headers&		headers;

	CGIResult(StatusCode code, const Headers& h)
		: status_code(code),
		headers(h) {}

};

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
	
	ResponseParser builder;

	Channel stdin_ch;
	Channel stdout_ch;
	Channel stderr_ch;

	runtime::epoll::EventPoller& poller_;
	Context& protocol_;
	
	CgiHandler(const CgiHandler&);
	CgiHandler& operator=(const CgiHandler&);

	void close_channel(Channel& channel);

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
	
	void on_ch_error(Channel& channel);
	void on_ch_closed(Channel& channel);
	void pause_channel(Channel::Stream type);
	void resume_channel(Channel::Stream type);
	
	CGIResult result() const;
};

}
