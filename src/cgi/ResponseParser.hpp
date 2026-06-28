#pragma once

#include "http/common/Headers.hpp"
#include "LineReader.hpp"
#include "StatusCode.hpp"

namespace http {

struct CGIResult {
	
	StatusCode	status_code;
	Headers		headers;

	CGIResult(
		StatusCode code,
		const Headers& h)
	: status_code(code),
	headers(h) {}

	CGIResult(StatusCode code): status_code(code) {}

	CGIResult() {}
};


struct CGIParseContext {
	static const std::size_t MaxHeaderBlockLen = 4096;

	LineReader 		line_reader;
	size_t			header_bytes;

	CGIParseContext(): line_reader(), header_bytes(0) {}
};

class ResponseParser {
public:
enum OutputState {
	Headers,
	Done,
	Error
};

enum ParseResult {
	Success,
	ParseError,
	Continue,
};

private:
	OutputState state_;

	http::StatusCode	code;
    http::Headers 		headers_;

	CGIParseContext parse_ctx;

	ParseResult parse_header(std::string const& line);
	ParseResult sanitize_status_header(std::string const& header);
	
public:
	ResponseParser();
	~ResponseParser();
	
	ParseResult	parse(BufferView& reader);
	bool		finished() const;

	CGIResult result() const;
};


}
