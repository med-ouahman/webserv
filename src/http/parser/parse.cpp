
#include "http/parser/parse.hpp"
#include "http/Context.hpp"
#include "server/limits.hpp"

namespace http {

namespace parser {

static Error	check_size(ContextState state, usize read_bytes) {
	switch (state) {
		case REQUEST_LINE:
			return read_bytes > Limits::MAX_REQUEST_LINE_SIZE
				? ERR_BAD_REQUEST : ERR_NONE;
		case HEADERS:
			return read_bytes > Limits::MAX_HEADER_SIZE
				? ERR_HEADER_TOO_LARGE : ERR_NONE;
		case BODY:
			return read_bytes > Limits::MAX_BODY_SIZE
				? ERR_BODY_TOO_LARGE : ERR_NONE;
		default:
			return ERR_NONE;
	}
}

Error	check_body_size(usize read_bytes) {
	return check_size(BODY, read_bytes);
}

}

Error Context::get_chunk(std::string& out, bool& found) {
	usize end = raw_buffer.find(CRLF);
	usize consumed;

	found = false;
	if (end == std::string::npos) {
		if (state_ == HEADERS)
			return parser::check_size(state_,
				header_bytes + raw_buffer.size());
		return parser::check_size(state_, raw_buffer.size());
	}
	consumed = end + 2;
	out = raw_buffer.substr(0, end);
	raw_buffer.erase(0, consumed);
	if (state_ == HEADERS)
		header_bytes += consumed;
	found = true;
	if (state_ == HEADERS)
		return parser::check_size(state_, header_bytes);
	return parser::check_size(state_, consumed);
}

Error Context::parse() {

	switch (state_) {
		case REQUEST_LINE:
			return parse_request_line();
		case HEADERS:
			return parse_headers();
		case BODY:
			return parse_body();
		default:
			return ERR_INTERNAL;
	}
}

}
