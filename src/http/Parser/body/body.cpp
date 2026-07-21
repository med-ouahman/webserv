#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::parseBody(Context& ctx) {
	if (ctx.actor.request.chunked)
		return parseChunkedBody(ctx);
	return parseFixedBody(ctx);
}

bool Parser::hasBody(const Request& request) const {
	bool res = false;
	base::Optional<usize> content_length = request.content_length;

	if (!request.chunked) {
		if ( content_length.has_value()
			and body_received == content_length.value )
				return true;
		return ( !raw_buffer.empty() );
	}

	switch (chunk_state) {
		case CHUNK_SIZE:
		case CHUNK_TRAILER:
			return raw_buffer.find(CRLF) != std::string::npos;
		case CHUNK_DATA:
			return !raw_buffer.empty();
		case CHUNK_CRLF:
			return raw_buffer.size() >= 2;
	}
	return res;
}

}
