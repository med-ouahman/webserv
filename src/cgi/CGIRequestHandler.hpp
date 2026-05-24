#pragma once

#include "CGIProcess.hpp"
#include "CGIEnvBuilder.hpp"
#include "CGIResponseParser.hpp"

namespace http {

struct ResolutionResult;
class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {

private:
	cgi::CGIProcess 		process;
	cgi::CGIEnvBuilder 		env_;
	cgi::CGIResponseParser parser;

	io::Stream 				stdout_;
	io::Stream 				stderr_;

	CGIRequestHandler( const CGIRequestHandler& );
	CGIRequestHandler& operator=( const CGIRequestHandler& );

public:
	CGIRequestHandler( const ResolutionResult result_ );
	~CGIRequestHandler();
	void handle();
	bool done();			
	void consume( DataView& view );
	void produce( BufferWriter& w );
	void on_stream_error();
	void on_stream_closed();
};

}
