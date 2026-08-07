
#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::chunkSizeState(BufferView& buff, usize& processed) {
	std::string line;
	bool found;
	Error err;

	TRY(getChunk(buff, line, processed, found), err);
	if (!found) {
		if (buff.remaining() > limits::CHUNK_SIZE_LINE_MAX)
			return ERR_BAD_REQUEST;
		return ERR_NONE;
	}
	if (line.size() > limits::CHUNK_SIZE_LINE_MAX)
		return ERR_BAD_REQUEST;
	TRY(parseChunkSize(line, chunk_size), err);
	chunk_received = 0;
	chunk_state = (chunk_size == 0) ? CHUNK_TRAILER : CHUNK_DATA;
	return ERR_NONE;
}

Error Parser::chunkDataState(BufferView& buff, usize& processed) {
	usize take;
	Error err;

	take = minSize(
		chunk_size - chunk_received,
		buff.remaining());
	if (take == 0)
		return ERR_NONE;
	TRY(bodyWrite(buff, take), err);
	chunk_received += take;
	buff.advance(take);
	processed += take;
	if (chunk_received == chunk_size)
		chunk_state = CHUNK_CRLF;
	return ERR_NONE;
}

Error Parser::chunkCrlfState(BufferView& buff, usize& processed) {
	if (buff.remaining() < 2)
			return ERR_NONE;
	if (buff.data()[0] != '\r' or buff.data()[1] != '\n')
			return ERR_BAD_REQUEST;
	buff.advance(2);
	processed += 2;
	chunk_size = 0;
	chunk_received = 0;
	chunk_state = CHUNK_SIZE;
	return ERR_NONE;
}

Error Parser::chunkTrailerState(Context& ctx, BufferView& buff, usize& processed) {
	std::string line;
	bool found;
	Error err;

	TRY(getChunk(buff, line, processed, found), err);
	if (!found)
		return ERR_NONE;
	if (line.empty())
		return finishBody(ctx);
	return ERR_NONE;
}

Error Parser::parseChunkedBody(Context& ctx, BufferView& buff, usize& processed) {
	Error err;

	while (hasBody(ctx.actor.request, buff)) {
			switch (chunk_state) {
					case CHUNK_SIZE:
							TRY(chunkSizeState(buff, processed), err);
							break;
					case CHUNK_DATA:
							TRY(chunkDataState(buff, processed), err);
							break;
					case CHUNK_CRLF:
							TRY(chunkCrlfState(buff, processed), err);
							break;
					case CHUNK_TRAILER:
							TRY(chunkTrailerState(ctx, buff, processed), err);
							break;
			}
	}
	return ERR_NONE;
}

}
