#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "Registrar.hpp"

namespace http {

struct ResolutionResult;
struct Request;

typedef void (*CGICOutputallback)(void* ctx);

struct CGIOutputContext {
	CGICOutputallback 	cb_;
	void* 				ctx;
};

struct CGIControl {
	RegisterContext register_ctx;
	CGIOutputContext output_ctx;
	CGIControl(RegisterContext ctx_, CGIOutputContext finished_)
		: register_ctx(ctx_), output_ctx(finished_) {}
};


class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {
public:
	enum State {
		BUILDING_HEADERS,
		HEADERS_READY,
		STREAMING_RESPONSE,
		RESPONSE_DONE,
		ERROR
	};

private:
	State state_;
	cgi::Process process;
	
	ResponseParser builder;

	io::Stream	stdin_stream;
	io::Stream 	stdout_stream;
	io::Stream 	stderr_stream;

	CGIControl	cgi_ctl;

	CGIRequestHandler(const CGIRequestHandler&);
	CGIRequestHandler& operator=(const CGIRequestHandler&);

public:
	CGIRequestHandler(const ResolutionResult& result, const http::Request& req, CGIControl& ctl);
	~CGIRequestHandler();

	State state() const;
	
	void handle();
	bool done();
	
	void consume(DataView& view);
	void produce(BufferWriter& w);
	void on_stream_error();
	void on_stream_closed();
};

}
