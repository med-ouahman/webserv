
#include "http/Context.hpp"
#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/routing/Routing.hpp"
#include "Logger.hpp"
#include "server/Server.hpp"

#include <sstream>

namespace http {

/* Data and actor initializations */
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

/* Context::consume() utility */
void Context::responseReady() {
	actor.response.resetWriteState();
	state_ = DONE;
	action_ = AC_WRITE;
	error = ERR_NONE;
}

Error Context::setError(Error error) {
	this->error = error;
	std::ostringstream ss;

	ss << "http: setError(" << error << ")";
	services_.logger.log(logger::Debug, ss.str(), false);

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

void Context::sessionConfigure() {
	Request& request = actor.request;
	bool sessionsEnabled = info.dispatch.value.server->session_enabled;

	if (!sessionsEnabled) {
		request.sessionsEnabled = false;
		request.session = NULL;
		return;
	}

	const std::string& cookie_name = info.dispatch.value.server->session_cookie_name;
	SessionManager* session = Server::find_session(services_.sessions, cookie_name);

	if (!session) {
		request.sessionsEnabled = false;
		request.session = NULL;
		return;
	}

	request.sessionsEnabled = true;
	request.session = session;

	std::string sid = extract_cookie_value(request.headers, session->get_cookie_name());
	request.currentSessionID = sid;
	request.currentSessionValid = !sid.empty() && session->has_session(sid);
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

Error Context::createHandler() {
	base::Expected<ARequestHandler*, Error> created =
		http::createHandler(info.dispatch.value.handler_type, *this);

	if (!created)
		return setError(created.error());
	actor.handler = created.value();
	return ERR_NONE;
}

/* Context::produce() utility */
usize Context::handleResponseFailure(Error err) {
	error = err;
	state_ = ERROR;
	action_ = actor.response.started() ? AC_CLOSE : AC_WRITE;
	return 0;
}

}
