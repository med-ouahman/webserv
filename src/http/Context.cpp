
#include "http/Context.hpp"

#include "http/Parser/body/temp_storage.hpp"

#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"

#include "http/routing/Routing.hpp"

#include <algorithm>
#include <cstdio>
#include <sstream>

namespace http {

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
		case LENGTH_REQUIRED: return "Length Required";
		case PAYLOAD_TOO_LARGE: return "Payload Too Large";
		case INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case NOT_IMPLEMENTED: return "Not Implemented";
		case BAD_GATEWAY: return "Bad Gateway";
		case GATEWAY_TIMEOUT: return "Gateway timeout";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
}

/* fallback to http 1.1 when the version's un-identified */
static std::string serializeResponseHead(const Response& response, Version ver) {
	std::ostringstream out;
	std::map<std::string, std::string>::const_iterator it;

	if (ver == HTTP_1_0) {
		out << "HTTP/1.0 " << static_cast<int>(response.status)
			<< " " << statusMsg(response.status) << "\r\n";
	}
	else {
		out << "HTTP/1.1 " << static_cast<int>(response.status)
			<< " " << statusMsg(response.status) << "\r\n";
	}

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

Parser::Parser(const std::string& body_path)
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

// Context::Context()
// 	: parser(),
// 	  request(),
// 	  response(),
// 	  response_head_(),
// 	  route(),
// 	  handler_(NULL),
// 	  conn_id_(0),
// 	  request_id_(0),
// 	  response_head_offset_(0),
// 	  response_body_offset_(0),
// 	  error_(ERR_NONE),
// 	  state_(PARSING),
// 	  action_(AC_READ),
// 	  response_started_(false) {
// 	request.method = UNKNOWN;
// 	request.version = HTTP_UNKNOWN;
// 	request.connection = CONNECTION_DEFAULT;
// 	request.chunked = false;
// 	response.status = OK;
// }

Context::Context(const std::vector<const config::ServerConfig*>& srvs, usize conn_id, usize request_id)
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
	  action_(AC_READ),
	  response_started_(false),
	  servers(srvs) {
	request.method = UNKNOWN;
	request.version = HTTP_UNKNOWN;
	request.connection = CONNECTION_DEFAULT;
	request.chunked = false;
	response.status = OK;
}

Context::~Context() {
	if (parser.bodyWriter.file_created())
		std::remove(parser.bodyWriter.path().c_str());
	delete handler_;
}

void Context::responseReady() {
	response_head_.clear();
	response_head_offset_ = 0;
	response_body_offset_ = 0;
	response_started_ = false;
	state_ = DONE;
	action_ = AC_WRITE;
	error_ = ERR_NONE;
}

Error Context::setError(Error error) {
	error_ = error;
	state_ = ERROR;
	action_ = AC_WRITE;
	return error;
}

Error Context::routeRequest(const config::Config& config) {
	Decision partial;
	base::Expected<Decision, Error> result =
		http::route(request, config, &partial);

	if (!result) {
		if (partial.location != NULL)
			route = partial;
		return setError(result.error());
	}
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

Error Context::handleError() {
	ErrorHandler handler(*this, error_);

	delete handler_;
	handler_ = NULL;
	return handler.handle();
}

usize Context::consume(const char* data, usize size) {
	Error err;
	usize consumed;

	if (timedOut())
		return 0;
	if (data == NULL && size != 0) {
		setError(ERR_BAD_REQUEST);
		return 0;
	}
	if (action_ != AC_READ)
		return 0;

	parser.raw_buffer.reserve(parser.raw_buffer.size() + size);
	parser.raw_buffer.append(data, size);
	consumed = size;

	switch (state_) {
		case PARSING:
			err = parser.parse(*this);
			if (err != ERR_NONE)
				setError(err);
			if (state_ == PROCESSING && !route.has_value()) {
				TRY(routeRequest(conf), (setError(err), static_cast<usize>(0)));
				if (action_ == AC_READ) return 0;
			}

			break;
		case PROCESSING:
			if (!route.has_value() || !route.value.read_body
				|| request.body.type() != base::io::Reader::NONE)
				return 0;
			err = parser.parseBody(*this);
			if (err != ERR_NONE)
				setError(err);
			else if (request.body.type() == base::io::Reader::NONE)
				action_ = parser.progressBody(request) ? AC_WRITE : AC_READ;
			break;
		case DONE:
		case ERROR:
			return 0;
	}
	return consumed;
}

void Context::process(const config::Config& config) {
	Error err;

	switch (state_) {
		case PARSING:
			TRY(parser.parse(*this), (setError(err), void()));
			if (state_ == PROCESSING && !route.has_value())
				TRY(routeRequest(config), (setError(err), void()));
			break;
		case PROCESSING: {
			if (action_ == AC_READ) return ;
			if (handler_ == NULL) TRY(createHandler(), (setError(err), void()));
			TRY(readBody(), (setError(err), void()));
			if (action_ == AC_READ) return ;
			TRY(handler_->handle(), (setError(err), void()));
			responseReady();
			return ;
		}
		case DONE:
			return ;
		case ERROR: {
			TRY(handleError(), (setError(err), void()));
			return ;
		}
	}
}

ContextAction Context::nextAction() const { return action_; }

bool Context::timedOut() {
	if (action_ != AC_READ
		|| (state_ != PARSING && state_ != PROCESSING))
		return false;
	if (!parser.timedOut())
		return false;
	setError(ERR_REQUEST_TIMEOUT);
	return true;
}

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

	if (response_head_.empty())
		response_head_ = serializeResponseHead(response, request.version);
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

usize Context::handleResponseFailure(Error err) {
	error_ = err;
	state_ = ERROR;
	action_ = response_started_ ? AC_CLOSE : AC_WRITE;
	return 0;
}

usize Context::produce(char *buffer, usize size) {
	base::io::Writer writer(buffer, size);
	Error err;
	usize sent = 0;

	if (action_ != AC_WRITE)
		return 0;
	if (state_ != DONE)
		process(conf);
	if (state_ != DONE || action_ != AC_WRITE)
		return 0;
	TRY(writeResponse(writer, sent), (handleResponseFailure(err)));
	if (sent > 0) response_started_ = true;
	return sent;
}

}
