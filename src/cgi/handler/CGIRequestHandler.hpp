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
	Context& protocol_;
	
	CGIRequestHandler(const CGIRequestHandler&);
	CGIRequestHandler& operator=(const CGIRequestHandler&);

public:
	CGIRequestHandler(const ResolutionResult& result, const http::Request& req, ServerContext& server_ctx);

	~CGIRequestHandler();
	State state() const;
	
	void handle();
	bool finished();
	
	void on_writable(BufferWriter& writer, Channel::Stream s);
	void on_readable(BufferReader& reader, Channel::Stream s);
	
	void on_ch_error(Channel::Stream s);
	void on_ch_closed(Channel::Stream s);

};

}
