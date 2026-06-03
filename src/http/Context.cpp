
#include "http/Context.hpp"
#include "http/parser/parse.hpp"

namespace http {

Context::Context()
	: raw_buffer(),
	  request(),
	  response(),
	  parse_offset(0),
	  header_bytes(0),
	  body_received(0),
	  state_(REQUEST_LINE) {
	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.transfer_encoding = TE_NONE;
	request.connection = CONNECTION_DEFAULT;
	response.status = OK;
}

Error Context::consume(const char* data, usize size) {
	if (data == NULL && size != 0)
		return BAD_REQUEST;

	raw_buffer.reserve(raw_buffer.size() + size);
	raw_buffer.append(data, size);

	return parser::parse(*this);
}

}
