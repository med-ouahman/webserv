
#include "http/routing/RoutingInternal.hpp"

namespace http {

namespace {

static bool handlerReadsBody(RequestType type) {
	return type == CGI || type == UPLOAD || type == LOGIN;
}

}

DispatchInfo::DispatchInfo()
	: server(NULL),
	  location(NULL),
	  upload_path(),
	  cgi_path(NULL),
	  normalized_path(),
	  normalized_uri(),
	  filesystem_path(),
	  path_type(not_found),
	  max_body_size(0),
	  cgi_timeout(0),
	  handler_type(STATIC_FILE),
	  read_body(false) {}

static base::Expected<DispatchInfo, Error> routeSelectedServer(const Request& request,
		const config::ServerConfig& server, DispatchInfo* partial) {
	DispatchInfo decision;
	Error err;

	decision.server = &server;

	TRY(routing::checkMethodSupported(request.method), err);
	TRY(routing::pathNormalize(request.path, decision.normalized_path), err);
	decision.normalized_uri = decision.normalized_path;
	if (request.query.has_value())
		decision.normalized_uri += "?" + request.query.value;
	TRY(routing::findLocation(decision, server), err);
	decision.max_body_size = routing::bodyLimit(decision);
	decision.cgi_timeout = decision.location->cgi_timeout;
	if (partial != NULL)
		*partial = decision;
	TRY(routing::checkMethodAllowed(request.method, *decision.location), err);
	TRY(routing::checkBodyPolicy(request), err);
	TRY(routing::checkBodySize(request, decision.max_body_size), err);

	if (routing::hasRedirect(*decision.location)) {
		decision.handler_type = REDIRECT;
		return decision;
	}

	TRY(routing::fsBuildPath(server, *decision.location,
		decision.normalized_path, decision.filesystem_path), err);
	TRY(routing::fsInspectPath(decision.location->root.empty()
		? server.root : decision.location->root,
		decision.filesystem_path, decision.path_type), err);
	TRY(routing::setRequestType(request, decision), err);
	if (decision.handler_type == DIRECTORY
		&& request.path.size() > 1
		&& request.path[request.path.size() - 1] != '/') {
		decision.handler_type = REDIRECT;
		return decision;
	}
	TRY(routing::pathTypeCheck(decision), err);
	decision.read_body = routing::hasBody(request)
		&& handlerReadsBody(decision.handler_type);
	if (decision.handler_type == UPLOAD) {
		TRY(routing::checkUploadAllowed(*decision.location), err);
		TRY(routing::checkUploadFraming(request), err);
		TRY(routing::resolveUploadPath(decision), err);
	}
	return decision;
}

base::Expected<DispatchInfo, Error> route(const Request& request,
		const std::vector<const config::ServerConfig*>& servers,
		DispatchInfo* partial) {
	const config::ServerConfig* server = NULL;
	Error err;

	TRY(routing::decideServer(request, servers, server), err);
	return routeSelectedServer(request, *server, partial);
}

}
