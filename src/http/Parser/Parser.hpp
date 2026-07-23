
#pragma once

#include <string>

#include "base/base.hpp"
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
	std::string	raw_buffer;

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

	Error	getChunk(std::string& out, bool& found);
	Error	parseRequestLine(Context& ctx);
	Error	parseHeaders(Context& ctx);
	Error	parseBody(Context& ctx);
	Error	parseFixedBody(Context& ctx);
	Error	parseChunkedBody(Context& ctx);
	Error	chunkSizeState();
	Error	chunkDataState();
	Error	chunkCrlfState();
	Error	chunkTrailerState(Context& ctx);
	Error	finishBody(Context& ctx);
	Error	bodyWrite(usize size);

	bool	canProgress(const Request& request) const;
	bool	hasBody(const Request& request) const;

	void incrementBuffer(const char* data, usize size, usize& consumed);

	static usize minSize(usize a, usize b);
	static bool parseChunkSize(const std::string& line, usize& size);

public:
		Parser();

		Error progress(Context& ctx, const char* data, usize size,
			usize& consumed);

		void reset();
		void resetCycle();
		bool hasBufferedInput() const;
		bool timedOut() const;
	void startBody();
	Error prepareBodyStorage(const std::string& root, usize conn_id,
		usize request_id, usize max_size);
};

}
