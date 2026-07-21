
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
			return read_bytes > limits::BODY_MAX_SIZE
				? ERR_BODY_TOO_LARGE : ERR_NONE;
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
	  bodyWriter(std::string(), body_buffer, limits::BODY_BUFFER_SIZE),
	  leading_crlf(false) {}

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
	leading_crlf = false;

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

Error Parser::progress(Context& ctx, const char* data, usize size,
		usize& consumed) {
	Error err;

	incrementBuffer(data, size, consumed);
	if (!canProgress(ctx.actor.request))
		return ERR_NONE;

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

void Parser::incrementBuffer(const char* data, usize size, usize& consumed) {
	raw_buffer.reserve(raw_buffer.size() + size);
	raw_buffer.append(data, size);
	consumed = size;
}

bool Parser::canProgress(const Request& request) const {
	bool res = false;

	if (phase == PARSING_REQUEST_LINE  or phase == PARSING_HEADERS)
			res = raw_buffer.find(CRLF) != std::string::npos;
	if (phase == PARSING_BODY)
		res = hasBody(request);
	return res;
}

}
