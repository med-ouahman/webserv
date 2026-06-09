#pragma once

#include "http/common/Headers.hpp"
#include "LineScanner.hpp"
#include "StatusCode.hpp"

namespace http {

enum ParseResult {
	PARSE_SUCCESS,
	PARSE_ERROR,
	PARSE_CONTINUE,
};

struct CGIParseContext {
	static const std::size_t MAX_CGI_HEADER_BLOCK_LEN = 4096;
	LineScanner 	sc_;
	size_t			header_bytes_;
	
	enum CGIParseState {
		HEADERS,
		HEADERS_DONE,
		CGI_ERROR
	} state_;

	CGIParseContext(): sc_(), header_bytes_(0), state_(HEADERS) {}
};


class ResponseParser {
private:
	http::StatusCode code;
    http::Headers 	headers_;
    CGIParseContext parse_ctx;
	void sanitize_status_line( const std::pair<std::string, std::string>& header );
	void sanitize_header( std::pair<std::string, std::string>& header );
public:
	ResponseParser();
	~ResponseParser();
	ParseResult parse_headers( BufferReader& view );
    const http::Headers& headers() const;
    http::StatusCode		status_code() const;
	bool finished() const;
};


}
