
#pragma once

#include <string>

#include "base/base.hpp"
#include "http/Error.hpp"
#include "server/limits.hpp"

namespace http {

class Context;

enum ChunkState {
	CHUNK_SIZE,
	CHUNK_DATA,
	CHUNK_CRLF,
	CHUNK_TRAILER
};

struct ParserState {
	std::string	raw_buffer;

	usize header_bytes;
	usize body_received;
	usize chunk_size;
	usize chunk_received;

	ChunkState chunk_state;

	char body_buffer[Limits::BODY_BUFFER_SIZE];
	base::io::Writer body_writer;

	ParserState();
	ParserState(const std::string& body_path);

	Error	get_chunk(Context& ctx, std::string& out, bool& found);

	Error	parse(Context& ctx);
	Error	parse_request_line(Context& ctx);
	Error	parse_headers(Context& ctx);
	Error	parse_body(Context& ctx);
	Error	parse_fixed_body(Context& ctx);
	Error	parse_chunked_body(Context& ctx);
	Error	parse_chunk_size_state(Context& ctx);
	Error	parse_chunk_data_state();
	Error	parse_chunk_crlf_state();
	Error	parse_chunk_trailer_state(Context& ctx);
	Error	finish_body(Context& ctx);
	Error	body_write(usize size);
};

namespace parser {

Error	check_body_size(usize read_bytes);

}

}
