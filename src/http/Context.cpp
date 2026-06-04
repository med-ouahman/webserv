
#include "http/Context.hpp"
#include "http/parser/parse.hpp"

#include <sstream>

#define HTTP_TMP_DIR ".tmp"

namespace http {

namespace {

static std::string	body_tmp_path(usize conn_id, usize request_id) {
	std::ostringstream path;

	path << HTTP_TMP_DIR << "/body_" << conn_id << "_" << request_id << ".tmp";
	return path.str();
}

}

Context::Context()
	: raw_buffer(),
	  request(),
	  response(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  chunk_state(CHUNK_SIZE),
	  body_buffer(),
	  body_writer(std::string(), body_buffer, Limits::BODY_BUFFER_SIZE),
	  state_(REQUEST_LINE) {
	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.connection = CONNECTION_DEFAULT;
	request.chunked = false;
	response.status = OK;
}

Context::Context(usize conn_id, usize request_id)
	: raw_buffer(),
	  request(),
	  response(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  chunk_state(CHUNK_SIZE),
	  body_buffer(),
	  body_writer(body_tmp_path(conn_id, request_id),
		  body_buffer,
		  Limits::BODY_BUFFER_SIZE),
	  state_(REQUEST_LINE) {
	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.connection = CONNECTION_DEFAULT;
	request.chunked = false;
	response.status = OK;
}

Error Context::consume(const char* data, usize size) {
	if (data == NULL && size != 0)
		return ERR_BAD_REQUEST;

	raw_buffer.reserve(raw_buffer.size() + size);
	raw_buffer.append(data, size);

	return parse();
}

}
