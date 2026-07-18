
#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

#include <cstdio>

namespace http {
namespace parser {

static Error checkSize(ParserPhase phase, usize read_bytes) {
	switch (phase) {
		case PARSING_REQUEST_LINE:
			return read_bytes > limits::REQUEST_LINE_MAX_SIZE
				? ERR_BAD_REQUEST : ERR_NONE;
		case PARSING_HEADERS:
			return read_bytes > limits::HEADER_MAX_SIZE
				? ERR_HEADER_TOO_LARGE : ERR_NONE;
		case PARSING_BODY:
			return ERR_NONE;
		default:
			return ERR_NONE;
	}
}

}

Parser::Parser()
	: raw_buffer(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  max_body_size(limits::BODY_MAX_SIZE),
	  phase(PARSING_REQUEST_LINE),
	  chunk_state(CHUNK_SIZE),
	  timer(),
	  body_buffer(),
	  bodyWriter(std::string(), body_buffer, limits::BODY_BUFFER_SIZE) {}

Parser::Parser(const std::string& body_path)
	: raw_buffer(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  max_body_size(limits::BODY_MAX_SIZE),
	  phase(PARSING_REQUEST_LINE),
	  chunk_state(CHUNK_SIZE),
	  timer(),
	  body_buffer(),
	  bodyWriter(body_path, body_buffer, limits::BODY_BUFFER_SIZE) {}

void Parser::reset() {
	if (bodyWriter.file_created())
		std::remove(bodyWriter.path().c_str());
	raw_buffer.clear();
	header_bytes = 0;
	body_received = 0;
	chunk_size = 0;
	chunk_received = 0;
	max_body_size = limits::BODY_MAX_SIZE;
	phase = PARSING_REQUEST_LINE;
	chunk_state = CHUNK_SIZE;
	timer.update();
	bodyWriter.reset();
}

Error Parser::getChunk(std::string& out, bool& found) {
	usize end = raw_buffer.find(CRLF);
	usize consumed;

	found = false;
	if (end == std::string::npos) {
		if (phase == PARSING_HEADERS)
			return parser::checkSize(phase,
				header_bytes + raw_buffer.size());
		return parser::checkSize(phase, raw_buffer.size());
	}
	consumed = end + 2;
	out = raw_buffer.substr(0, end);
	raw_buffer.erase(0, consumed);
	if (phase == PARSING_HEADERS)
		header_bytes += consumed;
	found = true;
	if (phase == PARSING_HEADERS)
		return parser::checkSize(phase, header_bytes);
	return parser::checkSize(phase, consumed);
}

Error Parser::parse(Context& ctx) {
	Error err;

	switch (phase) {
		case PARSING_REQUEST_LINE:
			err = parseRequestLine(ctx);
			break;
		case PARSING_HEADERS:
			err = parseHeaders(ctx);
			break;
		case PARSING_BODY:
			err = parseBody(ctx);
			break;
		default:
			return ERR_INTERNAL;
	}
	if (err == ERR_NONE && ctx.state_ == PARSING)
		ctx.action_ = progressParsing() ? AC_WRITE : AC_READ;
	return err;
}

bool Parser::timedOut() const {
	switch (phase) {
		case PARSING_REQUEST_LINE:
			return timer.elapsed() >= timeout::REQUEST_LINE_SECONDS;
		case PARSING_HEADERS:
			return timer.elapsed() >= timeout::HEADER_SECONDS;
		case PARSING_BODY:
			return timer.elapsed() >= timeout::BODY_PROGRESS_SECONDS;
	}
	return false;
}

void Parser::startBody() {
	phase = PARSING_BODY;
	timer.update();
}

bool Parser::progressParsing() const {
	return phase != PARSING_BODY
		&& raw_buffer.find(CRLF) != std::string::npos;
}
}
