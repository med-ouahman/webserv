#pragma once

#include "CGIProcess.hpp"
#include "CGIEnvBuilder.hpp"
#include "CGIResponseBuilder.hpp"
#include "IRequestHandler.hpp"

namespace http {

struct ResolutionResult;
struct Request;

class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {

private:
	cgi::CGIProcess 		process_;
	cgi::CGIResponseBuilder builder_;

	io::Stream				stdin_;
	io::Stream 				stdout_;
	io::Stream 				stderr_;

	CGIRequestHandler( const CGIRequestHandler& );
	CGIRequestHandler& operator=( const CGIRequestHandler& );

public:
	CGIRequestHandler( const ResolutionResult& result_, const http::Request& req );
	~CGIRequestHandler();
	void handle();
	bool done();			
	void consume( DataView& view );
	void produce( BufferWriter& w );
	void on_stream_error();
	void on_stream_closed();
};

}
