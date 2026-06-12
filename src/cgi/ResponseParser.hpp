#pragma once

#include "http/common/Headers.hpp"
#include "LineScanner.hpp"
#include "StatusCode.hpp"

namespace http {

enum ParseResult {
	Success,
	Malformed,
	ParseError,
	Continue,
};

struct CGIParseContext {
	static const std::size_t MaxHeaderBlockLen = 4096;
	LineScanner 	scanner;
	size_t			header_bytes;
	
	enum CGIParseState {
		Headers,
		Done,
		Error
	} state_;

	CGIParseContext(): scanner(), header_bytes(0), state_(Headers) {}
};


class ResponseParser {
private:
	http::StatusCode code;
	std::string		reason;
    http::Headers 	headers_;
    CGIParseContext parse_ctx;

	ParseResult parse_header(std::string const& line);
	ParseResult sanitize_status_header(std::string const& header);

public:
	ResponseParser();
	~ResponseParser();
	ParseResult parse_headers( BufferReader& view );
    const http::Headers& headers() const;
    http::StatusCode		status_code() const;
	bool finished() const;
};


}
