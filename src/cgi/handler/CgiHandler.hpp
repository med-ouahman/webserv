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
	
	BufferReader& source_;

	StatusCode	status_code;	
	const Headers&		headers;

	CGIResult(BufferReader& source, StatusCode code, const Headers& h)
		: source_(source),
		status_code(code),
		headers(h) {}
};

class CgiHandler: public IRequestHandler {

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
	
	CgiHandler(const CgiHandler&);
	CgiHandler& operator=(const CgiHandler&);

	void close_channel(Channel::Stream type);

public:
	CgiHandler(const ResolutionResult& result,
		const http::Request& req,
		runtime::epoll::EventPoller& p,
		Context& ctx);

	~CgiHandler();
	State state() const;
	
	void handle();
	bool finished();
	
	void on_writable(BufferWriter& writer, Channel::Stream s);
	void on_readable(BufferReader& reader, Channel::Stream s);
	
	void on_ch_error(Channel::Stream s);
	void on_ch_closed(Channel::Stream s);

};

}
