
#include "http/Context.hpp"
#include "http/Parser/parser.hpp"
#include "http/routing/Routing.hpp"
#include "CGIRequestHandler.hpp"
#include "HandlerFactory.hpp"
#include <sstream>

#define HTTP_TMP_DIR ".tmp"

namespace http {

/*
namespace {

static std::string	body_tmp_path(usize conn_id, usize request_id) {
	std::ostringstream path;

	path << HTTP_TMP_DIR << "/body_" << conn_id << "_" << request_id << ".tmp";
	return path.str();
}

}
*/

ParserState::ParserState()
	: raw_buffer(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  chunk_state(CHUNK_SIZE),
	  body_buffer(),
	  body_writer(std::string(), body_buffer, Limits::BODY_BUFFER_SIZE) {}

ParserState::ParserState(const std::string& body_path)
	: raw_buffer(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  chunk_state(CHUNK_SIZE),
	  body_buffer(),
	  body_writer(body_path, body_buffer, Limits::BODY_BUFFER_SIZE) {}

Context::Context(ServerContext& serv_ctx)
	: parser(),
	  request(),
	  response(),
	  state_(REQUEST_LINE),
	  action_(AC_READ),
	  factory(serv_ctx) {

	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.connection = CONNECTION_DEFAULT;
	request.chunked = false;
	response.status = OK;
}

Error Context::consume(const char* data, usize size) {

	ResolutionResult r;
	handler = factory.create(r, request, HandlerFactory::Cgi);

	return ERR_NONE;
	
	if (data == NULL && size != 0)
		return ERR_BAD_REQUEST;
	
	Error err;
	action_ = AC_READ;
	parser.raw_buffer.reserve(parser.raw_buffer.size() + size);
	parser.raw_buffer.append(data, size);

	if (state_ == PROCESSING)
		err = parser.parse_body(*this);
	else
		err = parser.parse(*this);
	if (err != ERR_NONE) {
		state_ = ERROR;
		action_ = AC_CLOSE;
	}
	return err;
}

Error Context::process(const config::Config& config) {
	routing::Decision decision;
	bool has_body;

	if (state_ != PROCESSING || action_ != AC_WORK)
		return ERR_NONE;

	decision = routing::route(request, config);
	has_body = request.chunked
		|| (request.content_length.has_value()
			&& request.content_length.value > 0);

	if (has_body && request.body.type() == base::io::Reader::NONE) {
		if (decision.body_policy == routing::BODY_REJECT) {
			state_ = ERROR;
			action_ = AC_CLOSE;
			return ERR_BAD_REQUEST;
		}
		if (decision.body_policy == routing::BODY_ACCEPT) {
			action_ = AC_READ;
			return ERR_NONE;
		}
	}
	action_ = AC_WORK;
	return ERR_NONE;
}

ContextAction Context::next_action() const { return action_; }

}
