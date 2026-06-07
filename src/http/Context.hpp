
#pragma once

#include <string>

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Request.hpp"
#include "http/Response.hpp"
#include "http/parser/parse.hpp"

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

/* 
 * @raw_buffer:		raw HTTP bytes used to transfer data.
 * @Request:		parsed incrementally with consume().
 * @Response:		built by process(), serialized by produce().
 * @parse_offset:	current reading position in raw_buffer.
 * @header_bytes:	number of parsed bytes.
 * @body_received:	number of parsed bytes.
 *
 * Context's role consists of the following steps:
 * 		- parsing the received data into a 'Request' object.
 * 		- handling the request through pipeline.
 * 		- generating a 'Response' object.
 * 		- serializing the response into bytes -> handed over to the connection writer
 * */

class Context {

private:

	std::string	raw_buffer;
	Request		request;
	Response	response;

	usize parse_offset;
	usize header_bytes;
	usize body_received;

	ContextState state_;

	friend Error	parser::get_chunk(Context& ctx, std::string& out, bool& found);
	friend Error	parser::parse(Context& ctx);
	friend Error	parser::parse_request_line(Context& ctx);
	friend Error	parser::parse_headers(Context& ctx);
	friend Error	parser::parse_body(Context& ctx);
	friend usize	pending_size(Context& ctx);
	friend usize	consume_chunk(Context& ctx, std::string& out, usize end);
	friend usize	parsed_size(Context& ctx, usize consumed);


public:

	Context();

	Error consume(const char* data, usize size);
	Error process(const config::Config& config);
	Error produce(base::io::Writer& writer);

	ContextState state() const;

};

}
