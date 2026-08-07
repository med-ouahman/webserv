
#include "http/routing/RoutingInternal.hpp"
#include "http/Context.hpp"
#include "http/Error.hpp"

namespace http {
namespace routing {

Error checkMethodSupported(Method method) {
	return method == UNKNOWN ? ERR_METHOD_NOT_ALLOWED : ERR_NONE;
}

Error checkMethodAllowed(Method method,
		const config::LocationConfig& location) {
	std::set<std::string>::const_iterator it;

	if (location.allowed_methods.empty())
		return ERR_NONE;
	it = location.allowed_methods.begin();
	
	while (it != location.allowed_methods.end()) {
		if (methodOf(*it) == method)
			return ERR_NONE;
		++it;
	}
	return ERR_METHOD_NOT_ALLOWED;
}

bool hasBody(const Request& request) {
	return request.has_body;
}

Error checkBodyPolicy(const Request& request) {
	if (!hasBody(request))
		return ERR_NONE;
	if (request.method == GET || request.method == DELETE)
		return ERR_BAD_REQUEST;
	return ERR_NONE;
}

usize bodyLimit(const DispatchInfo& decision) {
	if (decision.location != NULL
		&& decision.location->client_max_body_size > 0)
		return decision.location->client_max_body_size;
	if (decision.server != NULL
		&& decision.server->client_max_body_size > 0)
		return decision.server->client_max_body_size;
	return limits::BODY_MAX_SIZE;
}

Error checkBodySize(const Request& request, usize max_body_size) {
	if (request.content_length.has_value()
		&& request.content_length.value > max_body_size)
		return ERR_BODY_TOO_LARGE;
	return ERR_NONE;
}

Error pathTypeCheck(const DispatchInfo& decision) {
	RequestType type = decision.handler_type;

	if (decision.path_type != not_found) return ERR_NONE;
	if (type == DIRECTORY or type == STATIC_FILE or type == CGI)
		return ERR_NOT_FOUND;
	return ERR_NONE;
}

}
}
