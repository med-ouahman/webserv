#include "http/Parser/body/body.hpp"
#include "http/Context.hpp"

namespace http {

Error ParserState::parse_fixed_body(Context& ctx) {
	usize expected;
	usize take;
	Error err;

	if (!ctx.request.content_length.has_value())
		return ERR_BAD_REQUEST;

	expected = ctx.request.content_length.value;
	if (body_received >= expected)
		return finish_body(ctx);

	take = parser::body_min_size(expected - body_received, raw_buffer.size());
	if (take == 0)
		return ERR_NONE;

	err = body_write(take);
	if (err != ERR_NONE)
		return err;
	if (body_received == expected)
		return finish_body(ctx);
	return ERR_NONE;
}

}
