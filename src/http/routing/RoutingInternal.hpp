
#pragma once

#include "http/routing/Routing.hpp"

namespace http {
namespace routing {

Error decideServer(const Request& request,
	const std::vector<const config::ServerConfig*>& servers,
	const config::ServerConfig*& server);

/* filesystem and path confirmation */
Error pathNormalize(const std::string& path, std::string& out);
Error fsBuildPath(const config::ServerConfig& server,
	const config::LocationConfig& location,
	const std::string& normalized_path,
	std::string& out);
Error fsInspectPath(const std::string& root,
	const std::string& path,
	PathType& type);
Error findLocation( DispatchInfo& decision,
					const config::ServerConfig& server );

Error checkMethodSupported(Method method);
Error checkMethodAllowed(Method method, const config::LocationConfig& location);
bool hasBody(const Request& request);
Error checkBodyPolicy(const Request& request);
usize bodyLimit(const config::ServerConfig& server);
Error checkBodySize(const Request& request, usize max_body_size);

bool hasRedirect(const config::LocationConfig& location);
Error checkUploadAllowed(const config::LocationConfig& location);
Error checkUploadFraming(const Request& request);
Error setRequestType(
		const Request& request,
		DispatchInfo& decision );

}
}
