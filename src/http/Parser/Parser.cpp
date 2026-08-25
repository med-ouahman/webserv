
#include "http/Parser/Parser.hpp"
#include "http/Parser/body/temp_storage.hpp"
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

static usize findCRLF(const char* str, usize size) {
	usize index = 0;

	while (index + 1 < size) {
		if (str[index] == '\r' and str[index + 1] == '\n')
			return index;
		++index;
	}
	return size;
}

}

Error Parser::getChunk(BufferView& buff, std::string& out,
			usize& processed, bool& found) {
	usize consumed;
	usize end = http::parser::findCRLF(buff.data(), buff.remaining());

	found = false;
	if (end == buff.remaining()) {
			if (phase == PARSING_HEADERS)
					return parser::checkSize(phase, header_bytes + buff.remaining());
			return parser::checkSize(phase, buff.remaining());
	}

	out.assign(buff.data(), end);
	consumed = end + 2;
	buff.advance(consumed);
	processed += consumed;
	if (phase == PARSING_HEADERS)
			header_bytes += consumed;
	found = true;
	if (phase == PARSING_HEADERS)
			return parser::checkSize(phase, header_bytes);
	return parser::checkSize(phase, consumed);
}

Parser::Parser()
	: header_bytes(0),
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

Error Parser::prepareBodyStorage(const std::string& root, usize conn_id,
		usize request_id, usize max_size) {
	max_body_size = max_size;
	if (!parser::prepareTempStorage(root)
		|| !bodyWriter.reset(parser::tempBodyPath(root, conn_id, request_id),
			body_buffer, limits::BODY_BUFFER_SIZE))
		return ERR_INTERNAL;

	phase = PARSING_BODY;
	timer.update();
	return ERR_NONE;
}

Error Parser::prepareBodyStorage(Context& ctx) {
	const DispatchInfo& dispatch = ctx.info.dispatch.value;
	const config::LocationConfig* location = dispatch.location;
	const config::ServerConfig* server = dispatch.server;

	if (dispatch.read_body
		and ctx.actor.request.body.type() == base::io::Reader::NONE) {
		const std::string& root = location->root.empty()
			? server->root
			: location->root;
			
		return prepareBodyStorage(root, ctx.info.conn_id,
			ctx.info.request_id, dispatch.max_body_size);
	}
	return ERR_NONE;
}

Error Parser::progress(Context& ctx,
	BufferView& buff,
	usize& processed) {
	Error err;

	while (true) {
		switch (phase) {
			case PARSING_REQUEST_LINE:
				TRY(parseRequestLine(ctx, buff, processed), err);
				if (phase == PARSING_REQUEST_LINE)
					return ERR_NONE;
				break;
			case PARSING_HEADERS:
				TRY(parseHeaders(ctx, buff, processed), err);
			
				if (ctx.state_ == PROCESSING) {
					SessionManager& session = SessionManager::instance();
					std::string sid
					= extract_cookie_value(ctx.actor.request.headers,
						session.get_cookie_name());
					bool valid = !sid.empty() && session.has_session(sid);
					
					ctx.actor.request.currentSessionID = sid;
					ctx.actor.request.currentSessionValid = valid;	
				}
				if (phase == PARSING_HEADERS)
					return ERR_NONE;
				break;
			case PARSING_BODY:
				TRY(parseBody(ctx, buff, processed), err);
				return ERR_NONE;
			default:
				return ERR_INTERNAL;
		}
	}
}

}
