#pragma once

#include "Process.hpp"
#include "IRequestHandler.hpp"
#include "ResponseParser.hpp"

namespace http {

struct ResolutionResult;
struct Request;

class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {

private:
	enum {
		BUILDING_RESPONSE,
		RESPONSE_READY,
		WRITING_RESPONSE,
	} state_;

	cgi::Process 				process_;
	cgi::parser::ResponseParser builder_;

	io::Stream				stdin_;
	io::Stream 				stdout_;
	io::Stream 				stderr_;

	CGIRequestHandler(const CGIRequestHandler&);
	CGIRequestHandler& operator=(const CGIRequestHandler&);

public:
	CGIRequestHandler(const ResolutionResult& result_, const http::Request& req);
	~CGIRequestHandler();
	void handle();
	bool done();	
	void consume(DataView& view);
	void produce(BufferWriter& w);
	void on_stream_error();
	void on_stream_closed();
};

}
