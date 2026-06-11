
#include "http/Parser/parser.hpp"
#include "http/Context.hpp"
#include "server/limits.hpp"

namespace http {

namespace parser {

static Error	check_size(ContextState state, usize read_bytes) {
	switch (state) {
		case REQUEST_LINE:
			return read_bytes > Limits::MAX_REQUEST_LINE_SIZE ? ERR_BAD_REQUEST : ERR_NONE;
		case HEADERS:
			return read_bytes > Limits::MAX_HEADER_SIZE ? ERR_HEADER_TOO_LARGE : ERR_NONE;
		default:
			return ERR_NONE;
	}
}

Error	check_body_size(usize read_bytes) {
	return read_bytes > Limits::MAX_BODY_SIZE ? ERR_BODY_TOO_LARGE : ERR_NONE;
}

}

Error ParserState::get_chunk(Context& ctx, std::string& out, bool& found) {
	usize end = raw_buffer.find(CRLF);
	usize consumed;

	found = false;
	if (end == std::string::npos) {
		if (ctx.state_ == HEADERS)
			return parser::check_size(ctx.state_,
				header_bytes + raw_buffer.size());
		return parser::check_size(ctx.state_, raw_buffer.size());
	}
	consumed = end + 2;
	out = raw_buffer.substr(0, end);
	raw_buffer.erase(0, consumed);
	if (ctx.state_ == HEADERS)
		header_bytes += consumed;
	found = true;
	if (ctx.state_ == HEADERS)
		return parser::check_size(ctx.state_, header_bytes);
	return parser::check_size(ctx.state_, consumed);
}

Error ParserState::parse(Context& ctx) {

	switch (ctx.state_) {
		case REQUEST_LINE:
			return parse_request_line(ctx);
		case HEADERS:
			return parse_headers(ctx);
		default:
			return ERR_INTERNAL;
	}
}

}
