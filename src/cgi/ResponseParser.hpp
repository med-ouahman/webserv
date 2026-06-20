#pragma once

#include "http/common/Headers.hpp"
#include "LineReader.hpp"
#include "StatusCode.hpp"

namespace http {

struct CGIResult {
	
	std::string body_filename;
	size_t		body_content_length;

	StatusCode	status_code;
	Headers		headers;

	CGIResult(
		std::string const& filename,
		size_t body_len,
		StatusCode code,
		const Headers& h)
	: body_filename(filename),
	body_content_length(body_len),
	status_code(code),
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
	Body,
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

	mutable int	body_fd;
	mutable std::string body_filename;
	size_t		body_content_length;

	ParseResult parse_header(std::string const& line);
	ParseResult sanitize_status_header(std::string const& header);
	ParseResult	read_body(BufferView& reader);

public:
	ResponseParser();
	~ResponseParser();
	
	ParseResult	parse(BufferView& reader);
	bool		finished() const;

	CGIResult result() const;
};


}
