
#include "base/base.hpp"
#include "http/Context.hpp"

#include "http/Parser/Parser.hpp"
#include "http/Parser/body/temp_storage.hpp"
#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/routing/Routing.hpp"
#include "http/Error.hpp"
#include <cstdio>

namespace http {

Info::Info(const std::vector<const config::ServerConfig*>& srvs,
		usize conn, usize req)
	: servers(srvs),
	  conn_id(conn),
	  request_id(req),
	  dispatch() {}

RequestCount::RequestCount()
	: active_cgi(0),
	  active_requests(0) {}

RequestCount Context::request_count;

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

Request::Request()
	: url(),
	  path(),
	  query(),
	  headers(),
	  host(),
	  content_length(),
	  body(),
	  method(UNKNOWN),
	  version(HTTP_UNKNOWN),
	  connection(CONNECTION_DEFAULT),
	  chunked(false) {}

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
}

Context::Context(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id, RuntimeServices& services)
	: actor(),
	  info(servers, conn_id, request_id),
	  error_(ERR_NONE),
	  state_(PARSING),
	  action_(AC_READ),
	  services_(services) {
	resetCycle();
	++request_count.active_requests;
}

Context::~Context() {
	actor.reset();
	if (request_count.active_requests > 0)
		--request_count.active_requests;
}

void Context::responseReady() {
	actor.response.resetWriteState();
	state_ = DONE;
	action_ = AC_WRITE;
	error_ = ERR_NONE;
}

Error Context::setError(Error error) {
	error_ = error;

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
	actor.response.body = "";
	responseReady();

	return error;
}

void Context::resetCycle() {
	actor.reset();
	info.dispatch = base::Optional<DispatchInfo>();
	error_ = ERR_NONE;
	state_ = PARSING;
	action_ = AC_READ;
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
	actor.parser.max_body_size = info.dispatch.value.max_body_size;
	if (info.dispatch.value.read_body
		&& actor.request.body.type() == base::io::Reader::NONE) {
		const std::string& root = info.dispatch.value.location->root.empty()
			? info.dispatch.value.server->root
			: info.dispatch.value.location->root;

		if (!parser::prepareTempStorage(root)
			|| !actor.parser.bodyWriter.reset(parser::tempBodyPath(root,
				info.conn_id, info.request_id), actor.parser.body_buffer,
				limits::BODY_BUFFER_SIZE))
			return setError(ERR_INTERNAL);
		actor.parser.startBody();
		if (!actor.parser.progressBody(actor.request))
			action_ = AC_READ;
	}
	return ERR_NONE;
}

Error Context::createHandler() {
	base::Expected<ARequestHandler*, Error> created =
		http::createHandler(info.dispatch.value.handlerType, *this);

	if (!created)
		return setError(created.error());
	actor.handler = created.value();
	return ERR_NONE;
}

Error Context::handleError() {
	ErrorHandler handler(*this, error_);

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
	if (action_ != AC_READ) return consumed;

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
		TRY(actor.parser.progress(*this, data, size, consumed),
				(setError(err), consumed));
		action_ = AC_NONE;
		return consumed;
	}

	return consumed;
}

void Context::process() {
	Error err;

	if (state_ != PROCESSING and action_ != AC_NONE) return ;
	TRY(actor.handler->handle(), (setError(err), void()));
	if (actor.handler->done()) responseReady();
}

ContextAction Context::nextAction() const { return action_; }

<<<<<<< HEAD
void Context::timeout() {
	if (state_ == PARSING and actor.parser.timedOut()) {
		setError(ERR_REQUEST_TIMEOUT);
		return ;
=======
bool Context::reconcile() {
	
	/* need a way to know if the current request is CGI TO call handler->monitor which is a cgi special case */
	if (info.dispatch.has_value() && info.dispatch.value.handlerType == CGI && actor.handler) {
		CgiHandler* h = static_cast<CgiHandler*>(actor.handler);
		http::Error err;
		h->monitor();
		TRY(actor.handler->handle(), (setError(err), false));
		
		if (h->done()) responseReady();

		if (h->can_close()) {
			delete h;
			h = NULL;
		}
>>>>>>> 31065a6de101d3f46f44397e26e96f6dd5dc9cbb
	}

	if (state_ == PROCESSING and actor.handler != NULL) {
		Error err = actor.handler->timeout();
		if (err != ERR_NONE) setError(err);
	}
}

usize Context::handleResponseFailure(Error err) {
	error_ = err;
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
	if (actor.response.finished()) {
		if (actor.response.shouldClose())
			action_ = AC_CLOSE;
		else
			resetCycle();
	}
	return sent;
}

}
