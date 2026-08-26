#include "http/routing/RoutingInternal.hpp"
#include <iostream>

namespace http {
namespace routing {

static bool matchPath(const std::string& path,
		const std::string& location) {
	if (location.empty())
		return false;
	if (location == "/")
		return !path.empty() && path[0] == '/';
	if (path.size() < location.size())
		return false;
	if (path.compare(0, location.size(), location) != 0)
		return false;
	return path.size() == location.size()
		|| location[location.size() - 1] == '/'
		|| path[location.size()] == '/';
}

Error findLocation( DispatchInfo& decision,
					const config::ServerConfig& server ) {
	const config::LocationConfig* best = NULL;
	usize i = 0;

	while (i < server.locations.size()) {
		const config::LocationConfig& location = server.locations[i];

		if (matchPath(decision.normalized_path, location.path)
			&& (best == NULL || location.path.size() > best->path.size())) {
			best = &location;
		}

		++i;
	}
	if (best == NULL)
		return ERR_NOT_FOUND;
	decision.location = best;
	return ERR_NONE;
}

}

}
