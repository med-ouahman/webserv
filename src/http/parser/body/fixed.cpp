#include "http/parser/body/body.hpp"
#include "http/Context.hpp"

namespace http {

Error Context::parse_fixed_body() {
	usize expected;
	usize take;
	Error err;

	if (!request.content_length.has_value())
		return ERR_BAD_REQUEST;

	expected = request.content_length.value;
	if (body_received >= expected)
		return finish_body();

	take = parser::body_min_size(expected - body_received, raw_buffer.size());
	if (take == 0)
		return ERR_NONE;

	err = body_write(take);
	if (err != ERR_NONE)
		return err;
	if (body_received == expected)
		return finish_body();
	return ERR_NONE;
}

}
