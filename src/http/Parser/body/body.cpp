#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

bool Parser::hasBody(const Request& request, BufferView& buff) const {
	base::Optional<usize> content_length = request.content_length;

	if (!request.chunked) {
			if (content_length.has_value()
					and body_received == content_length.value)
					return true;
			return !buff.empty();
	}

	switch (chunk_state) {
			case CHUNK_SIZE:
			case CHUNK_TRAILER: {
				const char* data = buff.data();
				usize size = buff.remaining();
				usize i = 0;

				while (i + 1 < size) {
					if (data[i] == '\r' and data[i + 1] == '\n')
						return true;
					++i;
				}
				return false;
			}
			case CHUNK_DATA: return !buff.empty();
			case CHUNK_CRLF: return buff.remaining() >= 2;
	}
	return false;
}

Error Parser::parseBody(Context& ctx, BufferView& buff, usize& processed) {
	Request& req = ctx.actor.request;

	if (!hasBody(req, buff))
		return ERR_NONE;
	if (req.chunked)
		return parseChunkedBody(ctx, buff, processed);
	return parseFixedBody(ctx, buff, processed);
}

}
