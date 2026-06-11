#include "http/Parser/body/body.hpp"
#include "http/Context.hpp"

namespace http {

Error ParserState::parse_body(Context& ctx) {
	if (ctx.request.chunked)
		return parse_chunked_body(ctx);
	return parse_fixed_body(ctx);
}

}
