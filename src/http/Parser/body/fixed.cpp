#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::parseFixedBody(Context& ctx, BufferView& buff, usize& processed) {
	usize expected;
	usize take;
	Error err;

	if (!ctx.actor.request.content_length.has_value())
		return ERR_BAD_REQUEST;

	expected = ctx.actor.request.content_length.value;
	if (body_received == expected)
		return finishBody(ctx);

	take = minSize(expected - body_received, buff.remaining());
	if (take == 0)
		return ERR_NONE;

	TRY(bodyWrite(buff, take), err);
	buff.advance(take);
	processed += take;
	if (body_received == expected)
		return finishBody(ctx);
	return ERR_NONE;
}

}
