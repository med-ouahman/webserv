
#include "http/routing/RoutingInternal.hpp"

namespace http {

DispatchInfo::DispatchInfo()
	: server(NULL),
	  location(NULL),
	  upload_path(NULL),
	  cgi_path(NULL),
	  normalized_path(),
	  filesystem_path(),
	  path_type(not_found),
	  max_body_size(0),
	  cgi_timeout(0),
	  handlerType(STATIC_FILE),
	  read_body(false) {}

static base::Expected<DispatchInfo, Error> routeSelectedServer(const Request& request,
		const config::ServerConfig& server, DispatchInfo* partial) {
	DispatchInfo decision;
	Error err;

	decision.server = &server;
	decision.max_body_size = routing::bodyLimit(server);

	TRY(routing::checkMethodSupported(request.method), err);
	TRY(routing::pathNormalize(request.path, decision.normalized_path), err);
	TRY(routing::findLocation(decision.normalized_path,
		server, decision.location), err);
	decision.cgi_timeout = decision.location->cgi_timeout;
	if (partial != NULL)
		*partial = decision;
	TRY(routing::checkMethodAllowed(request.method, *decision.location), err);
	TRY(routing::checkBodyPolicy(request), err);
	TRY(routing::checkBodySize(request, decision.max_body_size), err);

	decision.read_body = routing::hasBody(request);
	if (routing::hasRedirect(*decision.location)) {
		decision.handlerType = REDIRECT;
		return decision;
	}

	TRY(routing::fsBuildPath(server, *decision.location,
		decision.normalized_path, decision.filesystem_path), err);
	TRY(routing::fsInspectPath(decision.location->root.empty()
		? server.root : decision.location->root,
		decision.filesystem_path, decision.path_type), err);
	decision.handlerType = routing::getRequestType(request,
		decision.normalized_path, *decision.location,
		decision.path_type,
		&decision.cgi_path);
	if (decision.handlerType == UPLOAD) {
		TRY(routing::checkUploadAllowed(*decision.location), err);
		TRY(routing::checkUploadFraming(request), err);
		decision.upload_path = &decision.location->upload_path;
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
