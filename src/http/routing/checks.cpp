
#include "http/routing/RoutingInternal.hpp"
#include "http/Context.hpp"

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

usize bodyLimit(const config::ServerConfig& server) {
	if (server.client_max_body_size > 0)
		return server.client_max_body_size;
	return limits::BODY_MAX_SIZE;
}

Error checkBodySize(const Request& request, usize max_body_size) {
	if (request.content_length.has_value()
		&& request.content_length.value > max_body_size)
		return ERR_BODY_TOO_LARGE;
	return ERR_NONE;
}

}
}
