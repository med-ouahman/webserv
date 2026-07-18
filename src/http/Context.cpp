
#include "http/Context.hpp"
#include "http/Parser/Parser.hpp"
#include "http/Parser/body/temp_storage.hpp"
#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/routing/Routing.hpp"
#include <cstdio>

namespace http {

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

RequestCount::RequestCount()
	: active_cgi(0),
	  active_requests(0) {}

RequestCount Context::request_count;

Actor::Actor()
	: parser(),
	  request(),
	  response(),
	  handler(NULL) {}

Info::Info(const std::vector<const config::ServerConfig*>& sr,
usize c, usize req)
: servers(sr),
conn_id(c),
request_id(req)
{}

Context::Context(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id)
	: actor(),
	  info(servers, conn_id, request_id),
	  error_(ERR_NONE),
	  state_(PARSING),
	  action_(AC_READ) {
	actor.request.method = UNKNOWN;
	actor.request.version = HTTP_UNKNOWN;
	actor.request.connection = CONNECTION_DEFAULT;
	actor.request.chunked = false;
	++request_count.active_requests;
}

Context::~Context() {
	if (actor.parser.bodyWriter.file_created())
		std::remove(actor.parser.bodyWriter.path().c_str());
	delete actor.handler;
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
	state_ = ERROR;
	action_ = AC_WRITE;
	return error;
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

Error Context::readBody() {
	Error err;

	if (!info.dispatch.value.read_body
		|| actor.request.body.type() != base::io::Reader::NONE)
		return ERR_NONE;
	TRY(actor.parser.parseBody(*this), setError(err));
	if (actor.request.body.type() == base::io::Reader::NONE)
		action_ = actor.parser.progressBody(actor.request) ? AC_WRITE : AC_READ;
	return ERR_NONE;
}

Error Context::createHandler() {
	base::Expected<RequestHandler*, Error> created =
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
	usize consumed;

	if (reconcile())
		return 0;
	if (data == NULL && size != 0) {
		setError(ERR_BAD_REQUEST);
		return 0;
	}
	
	actor.parser.raw_buffer.reserve(actor.parser.raw_buffer.size() + size);
	actor.parser.raw_buffer.append(data, size);
	consumed = size;

	switch (state_) {
		case PARSING:
			err = actor.parser.parse(*this);
			if (err != ERR_NONE)
				setError(err);
			if (state_ == PROCESSING && !info.dispatch.has_value()) {
				TRY(resolveDispatch(), (setError(err), static_cast<usize>(0)));
				TRY(prepareBodyStorage(),
					(setError(err), static_cast<usize>(0)));
				if (action_ == AC_READ) return 0;
			}

			break;
		case PROCESSING:
			if (!info.dispatch.has_value() || !info.dispatch.value.read_body
				|| actor.request.body.type() != base::io::Reader::NONE)
				return 0;
			err = actor.parser.parseBody(*this);
			if (err != ERR_NONE)
				setError(err);
			else if (actor.request.body.type() == base::io::Reader::NONE)
				action_ = actor.parser.progressBody(actor.request) ? AC_WRITE : AC_READ;
			break;
		case DONE:
		case ERROR:
			return 0;
	}
	return consumed;
}

void Context::process() {
	Error err;

	switch (state_) {
		case PARSING:
			TRY(actor.parser.parse(*this), (setError(err), void()));
			if (state_ == PROCESSING && !info.dispatch.has_value()) {
				TRY(resolveDispatch(), (setError(err), void()));
				TRY(prepareBodyStorage(), (setError(err), void()));
			}
			break;
		case PROCESSING: {
			if (action_ == AC_READ) return ;
			if (actor.handler == NULL) TRY(createHandler(), (setError(err), void()));
			TRY(readBody(), (setError(err), void()));
			if (action_ == AC_READ) return ;
			TRY(actor.handler->handle(), (setError(err), void()));
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

bool Context::reconcile() {
	return false;
	if (action_ != AC_READ
		|| (state_ != PARSING && state_ != PROCESSING))
		return false;
	// if (!actor.parser.timedOut())
		// return false;
	setError(ERR_REQUEST_TIMEOUT);
	return true;
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

	if (action_ != AC_WRITE)
		return 0;
	if (state_ != DONE)
		process();
	if (state_ != DONE || action_ != AC_WRITE)
		return 0;
	TRY(actor.response.write(buffer, size, actor.request.version, sent),
		(handleResponseFailure(err)));
	if (actor.response.finished()) {
		if (actor.response.shouldClose())
			action_ = AC_CLOSE;
		else
			action_ = AC_READ;
	}
	return sent;
}

}
