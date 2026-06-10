#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"

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



class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {

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

	io::Stream	stdin_stream;
	io::Stream 	stdout_stream;
	io::Stream 	stderr_stream;

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
	
	void consume(BufferReader& view);
	void produce(BufferWriter& w);
	void on_stream_error();
	void on_stream_closed();
	
};

}
