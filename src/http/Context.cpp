
#include "base/base.hpp"
#include "http/Context.hpp"

#include "http/Parser/Parser.hpp"
#include "http/Parser/body/temp_storage.hpp"
#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/routing/Routing.hpp"
#include "http/Error.hpp"
#include "CgiHandler.hpp"

#include <cstdio>
#include <iostream>

namespace http {

Info::Info(const std::vector<const config::ServerConfig*>& srvs,
		usize conn, usize req)
	: servers(srvs),
	  conn_id(conn),
	  request_id(req),
	  dispatch() {}

Actor::Actor()
	: parser(),
	  request(),
	  response(),
	  handler(NULL) {
	reset();
}

	void Actor::reset() {
		delete handler;
		handler = NULL;
		parser.reset();
		request.reset();
		response.reset();
	}

	void Actor::resetCycle() {
		delete handler;
		handler = NULL;
		parser.resetCycle();
		request.reset();
		response.reset();
	}

Request::Request()
	: url(),
	  path(),
	  headers(),
	  query(),
	  host(),
	  content_length(),
	  body(),
	  method(UNKNOWN),
	  version(HTTP_UNKNOWN),
	  connection(CONNECTION_DEFAULT),
	  chunked(false),
	  has_body(false) {}

void Request::reset() {
	url.clear();
	path.clear();
	query = base::Optional<std::string>();
	headers.clear();
	host = base::Optional<std::string>();
	content_length = base::Optional<usize>();
	body.reset();
	method = UNKNOWN;
	version = HTTP_UNKNOWN;
	connection = CONNECTION_DEFAULT;
	chunked = false;
	has_body = false;
}

Context::Context(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id, RuntimeServices& services)
	: actor(),
	  info(servers, conn_id, request_id),
	  error(ERR_NONE),
	  active_requests(0),
	  services_(services) {
	resetCycle();
}

Context::~Context() { actor.reset(); }

void Context::responseReady() {
	actor.response.resetWriteState();
	state_ = DONE;
	action_ = AC_WRITE;
	error = ERR_NONE;
}

Error Context::setError(Error error) {
	this->error = error;

	actor.response.reset();
	delete actor.handler;

	actor.handler = new (std::nothrow) ErrorHandler(*this, error);
	if (actor.handler) {
		state_ = PROCESSING;
		action_ = AC_NONE;
		return error;
	}

	actor.handler = NULL;
	actor.response.status = INTERNAL_SERVER_ERROR;
	actor.response.body = "<!doctype html><html><body> \
<h1>500 Internal Server Error</h1></body></html>\n";
	responseReady();

	return error;
}

void Context::resetCycle() {
	bool has_buffered_input = actor.parser.hasBufferedInput();

	actor.resetCycle();
	info.dispatch = base::Optional<DispatchInfo>();
	error = ERR_NONE;
	state_ = PARSING;
	action_ = AC_READ;
	if (active_requests > limits::MAX_REQUESTS_PER_CONN)
		setError(ERR_TOO_MANY_REQUESTS);
	++active_requests;
	if (has_buffered_input && action_ == AC_READ)
		consume(NULL, 0);
}

void Context::advanceCycle() {
	if (!actor.response.finished())
			return;
	if (actor.response.shouldClose()) {
			action_ = AC_CLOSE;
			return;
	}
	resetCycle();
}

Error Context::resolveDispatch() {
	if (info.servers.empty())
		return setError(ERR_INTERNAL);
	DispatchInfo partial;
	base::Expected<DispatchInfo, Error> result =
		http::route(actor.request, info.servers, &partial);
	if (!result) {
		if (partial.location != NULL || partial.server != NULL)
			info.dispatch = partial;
		return setError(result.error());
	}
	info.dispatch = result.value();
	return ERR_NONE;
}

Error Context::prepareBodyStorage() {
	if (info.dispatch.value.read_body
		and actor.request.body.type() == base::io::Reader::NONE) {
		const std::string& root = info.dispatch.value.location->root.empty()
			? info.dispatch.value.server->root
			: info.dispatch.value.location->root;

		return actor.parser.prepareBodyStorage(root, info.conn_id,
			info.request_id, info.dispatch.value.max_body_size);
	}
	return ERR_NONE;
}

Error Context::createHandler() {
	/* don't create the handle yet if the request hasn't be full read */

	base::Expected<ARequestHandler*, Error> created =
		http::createHandler(info.dispatch.value.handler_type, *this);

	if (!created)
		return setError(created.error());
	actor.handler = created.value();
	return ERR_NONE;
}

Error Context::handleError() {
	ErrorHandler handler(*this, error);

	delete actor.handler;
	actor.handler = NULL;
	return handler.handle();
}

usize Context::consume(const char* data, usize size) {
	Error err;
	usize consumed = 0;

	if (data == NULL && size != 0) {
		setError(ERR_BAD_REQUEST);
		return consumed;
	}
	
	if (state_ == PARSING) {
		TRY(actor.parser.progress(*this, data, size, consumed),
				(setError(err), consumed));
		if (state_ == PROCESSING && !info.dispatch.has_value()) {
			TRY(resolveDispatch(), (setError(err), consumed));
			TRY(prepareBodyStorage(), (setError(err), consumed));
			TRY(createHandler(), (setError(err), consumed));
		}
		return consumed;
	}

	if (state_ == PROCESSING) {
		if (actor.parser.parsingBody()) {
			TRY(actor.parser.progress(*this, data, size, consumed),
					(setError(err), consumed));
		} else
			consumed = 0;
		action_ = AC_NONE;
		return consumed;
	}

	return consumed;
}

void Context::process() {
	Error err;

	if (state_ != PROCESSING or action_ != AC_NONE) return;
	TRY(actor.handler->handle(), (setError(err), void()));
}

ContextAction Context::nextAction() const { return action_; }

void Context::timeout() {
	
	if (state_ == PARSING and actor.parser.timedOut()) {
		setError(ERR_REQUEST_TIMEOUT);
		return ;
	}
	
	if (state_ == PROCESSING and actor.handler != NULL)
		actor.handler->monitor();
}

usize Context::handleResponseFailure(Error err) {
	error = err;
	state_ = ERROR;
	action_ = actor.response.started() ? AC_CLOSE : AC_WRITE;
	return 0;
}

usize Context::produce(char *buffer, usize size) {
	Error err;
	usize sent = 0;

	if (state_ != DONE || action_ != AC_WRITE) return 0;
	TRY(actor.response.write(buffer, size, actor.request.version, sent),
		(handleResponseFailure(err)));
	if (actor.response.finished() && actor.response.shouldClose())
			action_ = AC_CLOSE;
	return sent;
}

}
