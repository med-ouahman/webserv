
#pragma once

#include <string>

#include "foundation/baselib/Base.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Error.hpp"
#include "Config.hpp"

namespace http {

enum ContextState {
	REQUEST_LINE,
	HEADERS,
	BODY,
	CGI_RUNNING,
	PROCESSING,
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
 * @content_length:	expected body size.
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
	usize content_length;

	ContextState state_;

public:

	Error consume(const char* data, usize size);
	Error process(const config::ServerConfig& config);
	Error produce(Base::io::Writer& writer);

	ContextState state() const;
};

}
