
#include "http/parser/parse.hpp"
#include "http/Context.hpp"
#include "types.hpp"

#include <string>

namespace http {

static const usize REQUEST_LINE_MAX_SIZE = 1024 * 8;
static const usize HEADER_MAX_SIZE = 1024 * 32;
static const usize BODY_MAX_SIZE = 1024 * 1024;

static Error	check_size(http::ContextState state, usize read_bytes) {
	switch (state) {
		case REQUEST_LINE:
			return read_bytes > REQUEST_LINE_MAX_SIZE ? ERR_BAD_REQUEST : NONE;
		case HEADERS:
			return read_bytes > HEADER_MAX_SIZE ? HEADER_TOO_LARGE : NONE;
		case BODY:
			return read_bytes > BODY_MAX_SIZE ? BODY_TOO_LARGE : NONE;
		default:
			return NONE;
	}
}

static usize	pending_size(Context& ctx) {
	if (ctx.state_ == HEADERS)
		return ctx.header_bytes + ctx.raw_buffer.size();
	return ctx.raw_buffer.size();
}

static usize	consume_chunk(Context& ctx, std::string& out, usize end) {
	usize consumed = end + 2;

	out = ctx.raw_buffer.substr(0, end);
	ctx.raw_buffer.erase(0, consumed);
	ctx.parse_offset = 0;
	if (ctx.state_ == HEADERS)
		ctx.header_bytes += consumed;
	return consumed;
}

static usize	parsed_size(Context& ctx, usize consumed) {
	if (ctx.state_ == HEADERS)
		return ctx.header_bytes;
	return consumed;
}


namespace parser {

Error	get_chunk(Context& ctx, std::string& out, bool& found) {
	usize end = ctx.raw_buffer.find(CRLF);
	usize consumed;

	found = false;
	if (end == std::string::npos)
		return check_size(ctx.state_, pending_size(ctx));
	consumed = consume_chunk(ctx, out, end);
	found = true;
	return check_size(ctx.state_, parsed_size(ctx, consumed));
}

Error	parse(Context& ctx) {

	switch (ctx.state_) {
		case REQUEST_LINE:
			return parse_request_line(ctx);
		case HEADERS:
			return parse_headers(ctx);
		case BODY:
			return parse_body(ctx);
		default:
			return INTERNAL;
	}
}

}

}
