#pragma once

#include "http/common/Headers.hpp"
#include "LineReader.hpp"
#include "Response.hpp"

namespace http {

struct CGIResult {
	bool mem_;
	std::string body_;
	std::string body_filename;
	size_t		body_content_length;

	StatusCode	status_code;
	Headers		headers;

	CGIResult(
		std::string const& filename,
		size_t content_len,
		StatusCode code,
		const Headers& h)	
	: mem_(false),
	body_(""),
	body_filename(filename),
	body_content_length(content_len),
	status_code(code),
	headers(h) {}

	CGIResult(
		std::string const& body,
		StatusCode code,
		const Headers& h)
	: mem_(true),
	body_(body),
	body_filename(""),
	body_content_length(0),
	status_code(code),
	headers(h) {}

	CGIResult(StatusCode code): status_code(code) {}

	CGIResult() {}
};


struct CGIParseContext {
	static const std::size_t MaxHeaderBlockLen = 2048;

	LineReader 		line_reader;
	size_t			header_bytes;

	CGIParseContext(): line_reader(), header_bytes(0) {}
};

class ResponseParser {
public:
enum BodyMode {
	Mem,
	Disk
};

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
	static const std::size_t MaxBodyMemSize = 1024;
	OutputState state_;

	http::StatusCode	code;
    http::Headers 		headers_;

	CGIParseContext parse_ctx;

	mutable int	body_fd;
	mutable std::string body_filename;
	size_t		body_content_length;

	std::string body_;
	BodyMode body_mode_;

	ParseResult parse_header(std::string const& line);
	ParseResult sanitize_status_header(std::string const& header);
	ParseResult	read_body(BufferView& view);
	bool		validate_headers() const;
	
public:
	ResponseParser();
	~ResponseParser();
	
	ParseResult	parse(BufferView& reader);
	bool		finished() const;

	CGIResult result() const;
};


}
