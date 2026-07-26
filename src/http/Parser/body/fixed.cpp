#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::parseFixedBody(Context& ctx) {
	usize expected;
	usize take;
	Error err;

	std::cout << "Body is chunked\n";
	if (!ctx.actor.request.content_length.has_value())
		return ERR_BAD_REQUEST;

	expected = ctx.actor.request.content_length.value;
	std::cout << "content-length: " << expected << "\n";
	if (body_received == expected)
		return finishBody(ctx);

	take = minSize(expected - body_received, raw_buffer.size());
	if (take == 0)
		return ERR_NONE;

	err = bodyWrite(take);
	if (err != ERR_NONE)
		return err;
	if (body_received == expected)
		return finishBody(ctx);
	return ERR_NONE;
}

}
