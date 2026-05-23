#pragma once

#include "CGIProcess.hpp"
#include "IRequestHandler.hpp"
#include "CStringArray.hpp"
#include "LineScanner.hpp"

namespace http {

struct ResolutionResult;

enum CGIParseState {
	STATUS_LINE,
	HEADERS,
	HEADERS_DONE,
	CGI_ERROR
};

enum ParseResult {
	PARSE_SUCCESS,
	PARSE_ERROR,
	PARSE_CONTINUE,
};

struct CGIParseContext {
	static const std::size_t MAX_CGI_HEADER_BLOCK_LEN = 4096;
	LineScanner 	sc_;
	size_t			header_bytes_;
	CGIParseState 	state_;
};

class CGIRequestHandler: public io::IStreamDelegate, public IRequestHandler {

private:
	const ResolutionResult result;
	cgi::CGIProcess process;
	CGIParseContext ctx;
	Headers			headers_;
	io::Stream 		stdout_;
	io::Stream 		stderr_;

	/* envp */
	const static char* cgi_metadata[];
	const static char* stripped_headers[];

	CStringArray build_envp( const CGIRequestContext& ctx );
	char* 		transform( bool http_prefix, Headers::const_iterator& it );
	Headers 	build_metadata( const CGIRequestContext& context );
	static bool forbidden_header( const std::string& header_name );
	/* parsing cgi output */

	ParseResult parse_headers( DataView& view );

	void sanitize_status_line( const std::pair<std::string, std::string>& header );
	void sanitize_header( std::pair<std::string, std::string>& header );

	CGIRequestHandler( const CGIRequestHandler& );
	CGIRequestHandler& operator=( const CGIRequestHandler& );

public:
	CGIRequestHandler( const ResolutionResult result_ );
	void handle();
	bool done();			
	void consume( DataView& view );
	void produce( BufferWriter& w );
	void on_stream_error();
	void on_stream_closed();
	~CGIRequestHandler();
	cgi::CGIExecContext create_exec_context();
	const Headers& headers() const;
};

}
