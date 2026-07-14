#include "http/routing/RoutingInternal.hpp"

namespace http {
namespace routing {

static std::string hostName(const std::string& host) {
	std::string::size_type colon = host.find(':');

	if (colon == std::string::npos)
		return host;
	return host.substr(0, colon);
}

static bool hasServerName(const config::ServerConfig& server,
		const std::string& host) {
	usize i = 0;

	while (i < server.server_names.size()) {
		if (server.server_names[i] == host)
			return true;
		++i;
	}
	return false;
}

Error decideServer(const Request& request,
		const std::vector<const config::ServerConfig*>& servers,
		const config::ServerConfig*& server) {
	usize i = 0;
	std::string host;

	if (servers.empty())
		return ERR_INTERNAL;
	server = servers[0];
	if (!request.host.has_value())
		return request.version == HTTP_1_1 ? ERR_MISSING_HOST : ERR_NONE;
	host = hostName(request.host.value);
	while (i < servers.size()) {
		if (servers[i] != NULL && hasServerName(*servers[i], host)) {
			server = servers[i];
			return ERR_NONE;
		}
		++i;
	}
	return ERR_NONE;
}

}
}
