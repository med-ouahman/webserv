
#pragma once

#include <string>

#include "base/base.hpp"
#include "foundation/BufferView.hpp"
#include "http/Error.hpp"
#include "http/limits.hpp"
#include "foundation/Timestamp.hpp"

namespace http {

class Context;
struct Request;

enum ChunkState { CHUNK_SIZE, CHUNK_DATA, CHUNK_CRLF, CHUNK_TRAILER };

enum ParserPhase { PARSING_REQUEST_LINE, PARSING_HEADERS, PARSING_BODY };

class Parser {
private:

	usize header_bytes;
	usize body_received;
	usize chunk_size;
	usize chunk_received;
	usize max_body_size;

	ParserPhase phase;
	ChunkState chunk_state;
	Timestamp timer;

	char body_buffer[limits::BODY_BUFFER_SIZE];
	base::io::Writer bodyWriter;

	bool leading_crlf;

	Error getChunk(BufferView& buff, std::string& out,
			usize& processed, bool& found);

	Error	parseRequestLine(Context& ctx, BufferView& buff,
			usize& processed);
	Error	parseHeaders(Context& ctx, BufferView& buff,
			usize& processed);
	Error	parseBody(Context& ctx, BufferView& buff,
			usize& processed);
	Error	parseFixedBody(Context& ctx, BufferView& buff,
			usize& processed);
	Error	parseChunkedBody(Context& ctx, BufferView& buff,
			usize& processed);
	Error	chunkSizeState(BufferView& buff, usize& processed);
	Error	chunkDataState(BufferView& buff, usize& processed);
	Error	chunkCrlfState(BufferView& buff, usize& processed);
	Error	chunkTrailerState(Context& ctx, BufferView& buff,
			usize& processed);
	Error	finishBody(Context& ctx);
	Error	bodyWrite(BufferView& buff, usize size);
	Error	prepareBodyStorage(const std::string& root, usize conn_id,
			usize request_id, usize max_size);

	bool	hasBody(const Request& request, BufferView& buff) const;

	static usize minSize(usize a, usize b);
	static Error parseChunkSize(const std::string& line, usize& size);

public:
	Parser();

	Error progress(Context& ctx, BufferView& buff, usize& processed);
	Error prepareBodyStorage(Context& ctx);

	void reset();
	bool timedOut() const;
};

}
