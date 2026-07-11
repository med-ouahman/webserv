
#include "http/routing/RoutingInternal.hpp"

namespace http {

Decision::Decision()
	: location(NULL),
	  upload_path(NULL),
	  cgi_path(NULL),
	  normalized_path(),
	  filesystem_path(),
	  path_type(not_found),
	  max_body_size(0),
	  handlerType(STATIC_FILE),
	  read_body(false) {}

base::Expected<Decision, Error> route(const Request& request,
		const config::Config& config) {
	Decision decision;
	Error err;

	decision.max_body_size = routing::bodyLimit(config.server);

	TRY(routing::checkMethodSupported(request.method), err);
	TRY(routing::pathNormalize(request.path, decision.normalized_path), err);
	TRY(routing::findLocation(decision.normalized_path,
		config.server, decision.location), err);
	TRY(routing::checkMethodAllowed(request.method, *decision.location), err);
	TRY(routing::checkBodyPolicy(request), err);
	TRY(routing::checkBodySize(request, decision.max_body_size), err);

	decision.read_body = routing::hasBody(request);
	if (routing::hasRedirect(*decision.location)) {
		decision.handlerType = REDIRECT;
		return decision;
	}

	TRY(routing::fsBuildPath(config.server, *decision.location,
		decision.normalized_path, decision.filesystem_path), err);
	TRY(routing::fsInspectPath(decision.location->root.empty()
		? config.server.root : decision.location->root,
		decision.filesystem_path, decision.path_type), err);
	decision.handlerType = routing::getRequestType(request,
		decision.normalized_path, *decision.location,
		decision.path_type,
		&decision.cgi_path);
	if (decision.handlerType == UPLOAD) {
		TRY(routing::checkUploadAllowed(*decision.location), err);
		decision.upload_path = &decision.location->upload_path;
	}
	return decision;
}

}
