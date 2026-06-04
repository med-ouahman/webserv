
#pragma once

#include <string>

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/parser/body/body.hpp"
#include "http/parser/parse.hpp"
#include "server/limits.hpp"

#define CRLF "\r\n"

namespace http {

enum ContextState {
	REQUEST_LINE,
	HEADERS,
	BODY,
	PROCESSING,
	CGI_RUNNING,
	RESPONSE_READY,
	WRITING_RESPONSE,
	DONE,
	ERROR,
};

enum ChunkState {
	CHUNK_SIZE,
	CHUNK_DATA,
	CHUNK_CRLF,
	CHUNK_TRAILER
};

/*
 * @raw_buffer:		raw HTTP bytes used to transfer data.
 * @Request:		parsed incrementally with consume().
 * @Response:		built by process(), serialized by produce().
 *
 * @header_bytes:	number of parsed bytes.
 * @body_received:	number of parsed bytes.
 * @chunk_size:		current chunk target size.
 * @chunk_received:	bytes parsed from the current chunk.
 *
 * @body_buffer:	in-memory body writer storage.
 * @body_writer:	body output writer, backed by memory or a temp file.
 *
 * @chunk_state:	current chunked-body parsing step.
 * @state_:			current global HTTP step.
 */
class Context {

private:

	std::string	raw_buffer;
	Request		request;
	Response	response;

	usize header_bytes;
	usize body_received;
	usize chunk_size;
	usize chunk_received;

	ChunkState chunk_state;

	char body_buffer[Limits::BODY_BUFFER_SIZE];
	base::io::Writer body_writer;

	ContextState state_;

	/* Parser methods */
	Error	get_chunk(std::string& out, bool& found);

	Error	parse();
	Error	parse_request_line();
	Error	parse_headers();
	Error	parse_body();
	Error	parse_fixed_body();
	Error	parse_chunked_body();
	Error	parse_chunk_size_state();
	Error	parse_chunk_data_state();
	Error	parse_chunk_crlf_state();
	Error	parse_chunk_trailer_state();
	Error	finish_body();
	Error	body_write(usize size);

	/* Processing methods */

public:

	Context();
	Context(usize conn_id, usize request_id);

	Error consume(const char* data, usize size);
	Error process(const config::Config& config);
	Error produce(base::io::Writer& writer);

	ContextState state() const;
};

}
