
#include "http/parser/parse.hpp"
#include "http/Context.hpp"

#include <string>

namespace http {

namespace parser {

bool	get_chunk(Context& ctx, std::string& out) {

	usize end = ctx.raw_buffer.find(CRLF, ctx.parse_offset);

	if (end == std::string::npos)
		return false;
	out = ctx.raw_buffer.substr(ctx.parse_offset, end - ctx.parse_offset);
	ctx.parse_offset = end + 2;
	return true;
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
