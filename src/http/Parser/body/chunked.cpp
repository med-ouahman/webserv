#include "http/Parser/body/body.hpp"
#include "http/Context.hpp"
#include "server/limits.hpp"

namespace http {

Error ParserState::parse_chunk_size_state(Context& ctx) {
	std::string line;
	bool found;
	Error err;

	if (raw_buffer.size() > Limits::MAX_CHUNK_SIZE_LINE)
		return ERR_BAD_REQUEST;
	err = get_chunk(ctx, line, found);
	if (err != ERR_NONE || !found)
		return err;
	if (!parser::body_parse_chunk_size(line, chunk_size))
		return ERR_BAD_REQUEST;
	chunk_received = 0;
	if (chunk_size == 0) {
		chunk_state = CHUNK_TRAILER;
		return ERR_NONE;
	}
	chunk_state = CHUNK_DATA;
	return ERR_NONE;
}

Error ParserState::parse_chunk_data_state() {
	usize take;
	Error err;

	take = parser::body_min_size(
		chunk_size - chunk_received,
		raw_buffer.size());
	if (take == 0)
		return ERR_NONE;
	err = body_write(take);
	if (err != ERR_NONE)
		return err;
	chunk_received += take;
	if (chunk_received == chunk_size)
		chunk_state = CHUNK_CRLF;
	return ERR_NONE;
}

Error ParserState::parse_chunk_crlf_state() {
	if (raw_buffer.size() < 2)
		return ERR_NONE;
	if (raw_buffer[0] != '\r' || raw_buffer[1] != '\n')
		return ERR_BAD_REQUEST;
	raw_buffer.erase(0, 2);
	chunk_size = 0;
	chunk_received = 0;
	chunk_state = CHUNK_SIZE;
	return ERR_NONE;
}

Error ParserState::parse_chunk_trailer_state(Context& ctx) {
	std::string line;
	bool found;
	Error err;

	err = get_chunk(ctx, line, found);
	if (err != ERR_NONE || !found)
		return err;
	if (line.empty())
		return finish_body(ctx);
	return ERR_NONE;
}

Error ParserState::parse_chunked_body(Context& ctx) {
	switch (chunk_state) {
		case CHUNK_SIZE:
			return parse_chunk_size_state(ctx);
		case CHUNK_DATA:
			return parse_chunk_data_state();
		case CHUNK_CRLF:
			return parse_chunk_crlf_state();
		case CHUNK_TRAILER:
			return parse_chunk_trailer_state(ctx);
	}
	return ERR_NONE;
}

}
