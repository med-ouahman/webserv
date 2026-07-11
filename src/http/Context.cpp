
#include "http/Context.hpp"

#include "http/Parser/parser.hpp"
#include "http/Parser/body/temp_storage.hpp"

#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"

#include "http/routing/Routing.hpp"
<<<<<<< HEAD
#include "HandlerFactory.hpp"
=======

#include <algorithm>
#include <sstream>
>>>>>>> c3383fd341a1f883e60340eec580765bf0ee20ab

namespace http {
  
/*

namespace {

static const char* statusMsg(StatusCode code) {
	switch (code) {
		case OK: return "OK";
		case CREATED: return "Created";
		case NO_CONTENT: return "No Content";
		case MOVED_PERMANENTLY: return "Moved Permanently";
		case FOUND: return "Found";
		case SEE_OTHER: return "See Other";
		case TEMPORARY_REDIRECT: return "Temporary Redirect";
		case PERMANENT_REDIRECT: return "Permanent Redirect";
		case BAD_REQUEST: return "Bad Request";
		case FORBIDDEN: return "Forbidden";
		case NOT_FOUND: return "Not Found";
		case METHOD_NOT_ALLOWED: return "Method Not Allowed";
		case REQUEST_TIMEOUT: return "Request Timeout";
		case CONFLICT: return "Conflict";
		case PAYLOAD_TOO_LARGE: return "Payload Too Large";
		case INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case NOT_IMPLEMENTED: return "Not Implemented";
		case BAD_GATEWAY: return "Bad Gateway";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
}

static std::string serializeResponseHead(const Response& response) {
	std::ostringstream out;
	std::map<std::string, std::string>::const_iterator it;

	out << "HTTP/1.1 " << static_cast<int>(response.status)
		<< " " << statusMsg(response.status) << "\r\n";
	for (it = response.headers.begin(); it != response.headers.end(); ++it)
		out << it->first << ": " << it->second << "\r\n";
	out << "\r\n";
	return out.str();
}

static bool shouldClose(const Response& response) {
	std::map<std::string, std::string>::const_iterator it =
		response.headers.find("Connection");

	if (it == response.headers.end())
		return true;
	return it->second != "keep-alive";
}

}
*/

Parser::Parser()
	: raw_buffer(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  max_body_size(limits::BODY_MAX_SIZE),
	  phase(PARSING_REQUEST_LINE),
	  chunk_state(CHUNK_SIZE),
	  timer(),
	  body_buffer(),
	  bodyWriter(std::string(), body_buffer, limits::BODY_BUFFER_SIZE) {}

<<<<<<< HEAD
<<<<<<< HEAD
Context::Context(ServerContext& serv_ctx)
	: parser(),
	  request(),
	  response(),
	  state_(REQUEST_LINE),
	  action_(AC_READ),
	  handler(NULL),
	  factory(serv_ctx, *this) {

=======
ParserState::ParserState(const std::string& body_path)
=======
Parser::Parser(const std::string& body_path)
>>>>>>> c3383fd341a1f883e60340eec580765bf0ee20ab
	: raw_buffer(),
	  header_bytes(0),
	  body_received(0),
	  chunk_size(0),
	  chunk_received(0),
	  max_body_size(limits::BODY_MAX_SIZE),
	  phase(PARSING_REQUEST_LINE),
	  chunk_state(CHUNK_SIZE),
	  timer(),
	  body_buffer(),
	  bodyWriter(body_path, body_buffer, limits::BODY_BUFFER_SIZE) {}

Context::Context()
	: parser(),
	  request(),
	  response(),
	  response_head_(),
	  route(),
	  handler_(NULL),
	  conn_id_(0),
	  request_id_(0),
	  response_head_offset_(0),
	  response_body_offset_(0),
	  error_(ERR_NONE),
	  state_(PARSING),
	  action_(AC_READ) {
>>>>>>> 8e7c4116533be1dc064e7cfaf34184364f7d1026
	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.connection = CONNECTION_DEFAULT;
	request.chunked = false;
	response.status = OK;
}

<<<<<<< HEAD
Context::~Context() {
	
	if (handler) delete handler;

	handler = NULL;
=======
Context::Context(usize conn_id, usize request_id)
	: parser(),
	  request(),
	  response(),
	  response_head_(),
	  route(),
	  handler_(NULL),
	  conn_id_(conn_id),
	  request_id_(request_id),
	  response_head_offset_(0),
	  response_body_offset_(0),
	  error_(ERR_NONE),
	  state_(PARSING),
	  action_(AC_READ) {
	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.connection = CONNECTION_DEFAULT;
	request.chunked = false;
	response.status = OK;
>>>>>>> 8e7c4116533be1dc064e7cfaf34184364f7d1026
}

Context::~Context() { delete handler_; }

void Context::responseReady() {
	response_head_.clear();
	response_head_offset_ = 0;
	response_body_offset_ = 0;
	state_ = DONE;
	action_ = AC_WRITE;
	error_ = ERR_NONE;
}

<<<<<<< HEAD
Error Context::consume(const char* data, usize size) {
	
	Error err;
	
	if (data == NULL && size != 0)
<<<<<<< HEAD
		return ERR_BAD_REQUEST;
	
	if (!handler)
	{
		ResolutionResult r;
		handler = factory.create(r, request, HandlerFactory::Cgi);
	}
	
	return ERR_NONE;
=======
		return handle_error(*this, ERR_BAD_REQUEST);
=======
Error Context::setError(Error error) {
	error_ = error;
	state_ = ERROR;
	action_ = AC_WRITE;
	return error;
}

Error Context::routeRequest(const config::Config& config) {
	base::Expected<Decision, Error> result = http::route(request, config);

	if (!result)
		return setError(result.error());
	route = result.value();
	parser.max_body_size = route.value.max_body_size;
	if (route.value.read_body
		&& request.body.type() == base::io::Reader::NONE) {
		const std::string& root = route.value.location->root.empty()
			? config.server.root : route.value.location->root;

		if (!parser::prepareTempStorage(root)
			|| !parser.bodyWriter.reset(parser::tempBodyPath(root,
				conn_id_, request_id_), parser.body_buffer,
				limits::BODY_BUFFER_SIZE))
			return setError(ERR_INTERNAL);
		parser.startBody();
		if (!parser.progressBody(request))
			action_ = AC_READ;
	}
	return ERR_NONE;
}

Error Context::readBody() {
	Error err;

	if (!route.value.read_body
		|| request.body.type() != base::io::Reader::NONE)
		return ERR_NONE;
	TRY(parser.parseBody(*this), setError(err));
	if (request.body.type() == base::io::Reader::NONE)
		action_ = parser.progressBody(request) ? AC_WRITE : AC_READ;
	return ERR_NONE;
}

Error Context::createHandler() {
	base::Expected<RequestHandler*, Error> created =
		http::createHandler(route.value.handlerType, *this);

	if (!created)
		return setError(created.error());
	handler_ = created.value();
	return ERR_NONE;
}

usize Context::consume(const char* data, usize size) {
	Error err;
	usize consumed;

	if (data == NULL && size != 0) {
		setError(ERR_BAD_REQUEST);
		return 0;
	}
>>>>>>> c3383fd341a1f883e60340eec580765bf0ee20ab
	if (action_ != AC_READ)
		return 0;

>>>>>>> 8e7c4116533be1dc064e7cfaf34184364f7d1026
	parser.raw_buffer.reserve(parser.raw_buffer.size() + size);
	parser.raw_buffer.append(data, size);
	consumed = size;

	switch (state_) {
		case PARSING:
			err = parser.parse(*this);
			if (err != ERR_NONE)
				setError(err);
			break;
		case PROCESSING:
			if (!route.has_value() || !route.value.read_body
				|| request.body.type() != base::io::Reader::NONE)
				return 0;
			err = parser.parseBody(*this);
			if (err != ERR_NONE)
				setError(err);
			else
				action_ = parser.progressBody(request) ? AC_WRITE : AC_READ;
			break;
		case DONE:
		case ERROR:
			return 0;
	}
	return consumed;
}

<<<<<<< HEAD
Error Context::process(const config::Config& config) {
<<<<<<< HEAD
	
	routing::Decision decision;
	bool has_body;
=======
=======
void Context::process(const config::Config& config) {
>>>>>>> c3383fd341a1f883e60340eec580765bf0ee20ab
	Error err;
>>>>>>> 8e7c4116533be1dc064e7cfaf34184364f7d1026

	switch (state_) {
		case PARSING:
			TRY(parser.parse(*this), (setError(err), void()));
			break;
		case PROCESSING: {
			if (!route.has_value()) {
				TRY(routeRequest(config), (setError(err), void()));
				if (action_ == AC_READ) return ;
			}
			TRY(readBody(), (setError(err), void()));
			if (action_ == AC_READ) return ;
			if (handler_ == NULL) TRY(createHandler(), (setError(err), void()));
			TRY(handler_->handle(), (setError(err), void()));
			responseReady();
			return ;
		}
		case DONE:
			return ;
		case ERROR: {
			ErrorHandler handler(*this, error_);

			handler.handle();
			return ;
		}
	}
}

ContextAction Context::nextAction() const { return action_; }

Error Context::writeResponse(base::io::Writer& writer, usize& sent) {
	Error err;

	TRY(writeResponseHead(writer, sent), err);
	if (response_head_offset_ != response_head_.size())
		return ERR_NONE;
	TRY(writeResponseBody(writer, sent), err);
	if (response_head_offset_ == response_head_.size()
		&& response.body_reader.type() == base::io::Reader::NONE
		&& response_body_offset_ == response.body.size()) {
		if (shouldClose(response))
			action_ = AC_CLOSE;
		else
			action_ = AC_READ;
	}
	return ERR_NONE;
}

Error Context::writeResponseHead(base::io::Writer& writer, usize& sent) {
	usize remaining;
	usize amount;

<<<<<<< HEAD
<<<<<<< HEAD
Error Context::produce(BufferWriter& w) {

	ssize_t n = response.encoder.encode(response.body, w);

	if (n == 0) {
		std::cout << "Closed\n";
		action_ = AC_CLOSE;
	}
	
	if (n < 0) {
		action_ = AC_CLOSE;
	}

	return ERR_NONE;
}


void Context::refresh_state() {

	if (!handler) {
		return;
	}

	CgiHandler* h = static_cast<CgiHandler*>(handler);
	h->refresh_state();
	
	if (h->finished()) {
		delete h;
		h = NULL;
		handler = NULL;
		std::cout << "Deleting the CGI handler\n";
	}
}

ContextAction Context::next_action() const { return action_; }
=======
void Context::response_ready() { action_ = AC_WRITE; }
>>>>>>> 8e7c4116533be1dc064e7cfaf34184364f7d1026
=======
	if (response_head_.empty())
		response_head_ = serializeResponseHead(response);
	remaining = response_head_.size() - response_head_offset_;
	if (remaining == 0)
		return ERR_NONE;
	amount = std::min(remaining, writer.freeSpace());
	if (amount == 0)
		return ERR_NONE;
	base::Expected<usize, base::io::Error> written =
		writer.write(response_head_.data() + response_head_offset_, amount);
	if (!written)
		return ERR_INTERNAL;
	response_head_offset_ += written.value();
	sent += written.value();
	return ERR_NONE;
}

Error Context::writeResponseBody(base::io::Writer& writer, usize& sent) {
	usize remaining;
	usize amount;

	if (response.body_reader.type() != base::io::Reader::NONE) {
		if (writer.freeSpace() == 0)
			return ERR_NONE;
		base::Expected<usize, base::io::Error> chunk =
			response.body_reader.read(writer.writePtr(), writer.freeSpace());
		if (!chunk)
			return ERR_INTERNAL;
		if (chunk.value() == 0) {
			if (shouldClose(response))
				action_ = AC_CLOSE;
			else
				action_ = AC_READ;
			return ERR_NONE;
		}
		if (!writer.commit(chunk.value()))
			return ERR_INTERNAL;
		sent += chunk.value();
		return ERR_NONE;
	}
	remaining = response.body.size() - response_body_offset_;
	if (remaining == 0)
		return ERR_NONE;
	amount = std::min(remaining, writer.freeSpace());
	if (amount == 0)
		return ERR_NONE;
	base::Expected<usize, base::io::Error> written =
		writer.write(response.body.data() + response_body_offset_, amount);
	if (!written)
		return ERR_INTERNAL;
	response_body_offset_ += written.value();
	sent += written.value();
	if (response_body_offset_ == response.body.size()) {
		if (shouldClose(response))
			action_ = AC_CLOSE;
		else
			action_ = AC_READ;
	}
	return ERR_NONE;
}

usize Context::produce(base::io::Writer& writer) {
	Error err;
	usize sent = 0;

	if (action_ != AC_WRITE)
		return 0;
	if (state_ != DONE)
		process(config::Config::get_config());
	if (state_ != DONE || action_ != AC_WRITE)
		return 0;
	TRY(writeResponse(writer, sent), (setError(err), static_cast<usize>(0)));
	return sent;
}
>>>>>>> c3383fd341a1f883e60340eec580765bf0ee20ab

}
