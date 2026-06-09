#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"
#include "Registrar.hpp"
#include "StatusCode.hpp"
#include "IBodyProvider.hpp"

namespace http {

struct ResolutionResult;
struct Request;

struct CGIResult {
	StatusCode code;
	std::string status_reason;
	Headers headers;
	IBodyProvider* body;

	CGIResult(): code(OK), status_reason(), headers(), body(NULL) {}
};

typedef void (*CGICOutputallback)(void* ctx, CGIResult const& result);

struct CGIOutputContext {
	CGICOutputallback 	cb_;
	void* 				ctx;
};

struct CGIControl {
	RegisterContext register_ctx;
	CGIOutputContext output_ctx;

	CGIControl(RegisterContext ctx_, CGIOutputContext output_ctx)
		: register_ctx(ctx_), output_ctx(output_ctx) {}
};

class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {

public:
	enum State {
		BUILDING_HEADERS,
		HEADERS_READY,
		STREAMING_BODY,
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

	CGIResult result_;

	CGIRequestHandler(const CGIRequestHandler&);
	CGIRequestHandler& operator=(const CGIRequestHandler&);

public:
	CGIRequestHandler(const ResolutionResult& result, const http::Request& req, CGIControl& ctl);
	~CGIRequestHandler();
	State state() const;
	
	void handle();
	bool done();
	
	void consume(BufferReader& view);
	void produce(BufferWriter& w);
	void on_stream_error();
	void on_stream_closed();

	CGIResult& result();
};

}
