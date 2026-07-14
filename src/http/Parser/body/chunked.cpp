
#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::chunkSizeState() {
	std::string line;
	bool found;
	Error err;
	usize end;

	end = raw_buffer.find(CRLF);
	if ((end == std::string::npos
		&& raw_buffer.size() > limits::CHUNK_SIZE_LINE_MAX)
		|| (end != std::string::npos
			&& end > limits::CHUNK_SIZE_LINE_MAX))
		return ERR_BAD_REQUEST;
	err = getChunk(line, found);
	if (err != ERR_NONE || !found)
		return err;
	if (!parseChunkSize(line, chunk_size))
		return ERR_BAD_REQUEST;
	if (body_received > max_body_size
		|| chunk_size > max_body_size - body_received)
		return ERR_BODY_TOO_LARGE;
	chunk_received = 0;
	if (chunk_size == 0) {
		chunk_state = CHUNK_TRAILER;
		return ERR_NONE;
	}
	chunk_state = CHUNK_DATA;
	return ERR_NONE;
}

Error Parser::chunkDataState() {
	usize take;
	Error err;

	take = minSize(
		chunk_size - chunk_received,
		raw_buffer.size());
	if (take == 0)
		return ERR_NONE;
	err = bodyWrite(take);
	if (err != ERR_NONE)
		return err;
	chunk_received += take;
	if (chunk_received == chunk_size)
		chunk_state = CHUNK_CRLF;
	return ERR_NONE;
}

Error Parser::chunkCrlfState() {
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

Error Parser::chunkTrailerState(Context& ctx) {
	std::string line;
	bool found;
	Error err;

	err = getChunk(line, found);
	if (err != ERR_NONE || !found)
		return err;
	if (line.empty())
		return finishBody(ctx);
	return ERR_NONE;
}

Error Parser::parseChunkedBody(Context& ctx) {
	switch (chunk_state) {
		case CHUNK_SIZE:
			return chunkSizeState();
		case CHUNK_DATA:
			return chunkDataState();
		case CHUNK_CRLF:
			return chunkCrlfState();
		case CHUNK_TRAILER:
			return chunkTrailerState(ctx);
	}
	return ERR_NONE;
}

}
