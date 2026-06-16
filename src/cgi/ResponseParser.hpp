#pragma once

#include "http/common/Headers.hpp"
#include "LineReader.hpp"
#include "StatusCode.hpp"

namespace http {

enum ParseResult {
	Success,
	ParseError,
	Continue,
};

struct CGIResult {

	std::string body_filename;
	size_t		body_content_length;

	StatusCode			status_code;
	const Headers&		headers;

	CGIResult(
		std::string const& filename,
		size_t body_len,
		StatusCode code,
		const Headers& h)
	: body_filename(filename),
	body_content_length(body_len),
	status_code(code),
	headers(h) {}
};


struct CGIParseContext {
	static const std::size_t MaxHeaderBlockLen = 4096;

	LineReader 		scanner;
	size_t			header_bytes;

	CGIParseContext(): scanner(), header_bytes(0) {}
};

class ResponseParser {
public:
enum OutputState {
	Headers,
	Body,
	Done,
	Error
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
	ParseResult	read_body(BufferReader& reader);

public:
	ResponseParser();
	~ResponseParser();
	ParseResult parse_headers(BufferReader& reader);

    const http::Headers&	headers() const;
    http::StatusCode		status_code() const;
	bool					finished() const;

	CGIResult result() const;
};


}
