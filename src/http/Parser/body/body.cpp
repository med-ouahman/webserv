#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::parseBody(Context& ctx) {
	if (ctx.actor.request.chunked)
		return parseChunkedBody(ctx);
	return parseFixedBody(ctx);
}

bool Parser::progressBody(const Request& request) const {
	if (!request.chunked)
		return !raw_buffer.empty()
			|| (request.content_length.has_value()
				&& body_received >= request.content_length.value);
	switch (chunk_state) {
		case CHUNK_SIZE:
		case CHUNK_TRAILER:
			return raw_buffer.find(CRLF) != std::string::npos;
		case CHUNK_DATA:
			return !raw_buffer.empty();
		case CHUNK_CRLF:
			return raw_buffer.size() >= 2;
	}
	return false;
}

}
