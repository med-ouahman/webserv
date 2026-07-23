
#include "http/routing/RoutingInternal.hpp"
#include <iostream>

namespace http {
namespace routing {

bool hasRedirect(const config::LocationConfig& location) {
	return location.redirect.return_code != 0
		&& !location.redirect.return_target.empty();
}

static bool extensionMatches(const std::string& path,
		const std::string& extension) {
	if (extension.empty() || path.size() < extension.size())
		return false;
	return path.compare(path.size() - extension.size(), extension.size(),
		 extension) == 0;
}

static bool pathStartsWith(const std::string& path, const std::string& dir) {
	usize len = dir.size();

	if (dir.empty())
			return false;
	if (path.compare(0, len, dir) != 0)
			return false;
	return path.size() == len || path[len] == '/';
}

static bool isInsideCgiDir(const DispatchInfo& decision) {
	std::string cgi_dir;
	std::string normalized_dir;

	if (decision.location->cgi_dir.empty())
		return false;
	cgi_dir = decision.location->cgi_dir;
	if (cgi_dir[0] != '/')
		cgi_dir.insert(0, "/");
	if (pathNormalize(cgi_dir, normalized_dir) != ERR_NONE)
		return false;
	if (fsBuildPath(*decision.server, *decision.location,
			normalized_dir, cgi_dir) != ERR_NONE)
		return false;
	return pathStartsWith(decision.filesystem_path, cgi_dir);
}

static const std::string* findCgiPath(const DispatchInfo& decision) {
	const config::LocationConfig& location = *decision.location;
	std::map<std::string, std::string>::const_iterator it;

	it = location.cgi_pass.begin();
	while (it != location.cgi_pass.end()) {
		if (extensionMatches(decision.normalized_path, it->first))
			return &it->second;
		++it;
	}
	return NULL;
}

Error checkUploadAllowed(const config::LocationConfig& location) {
	if (!location.upload_enabled || location.upload_path.empty())
		return ERR_FORBIDDEN;
	return ERR_NONE;
}

Error checkUploadFraming(const Request& request) {
	if (!request.chunked and !request.content_length.has_value())
		return ERR_LENGTH_REQUIRED;
	return ERR_NONE;
}

Error setRequestType(const Request& request,
		DispatchInfo& decision) {
	decision.cgi_path = findCgiPath(decision);
	if (decision.cgi_path != NULL) {
		decision.handler_type = CGI;
		return ERR_NONE;
	}
	if (isInsideCgiDir(decision)) {
		if (decision.path_type != executable)
			return ERR_FORBIDDEN;
		decision.handler_type = CGI;
		return ERR_NONE;
	}
	if (request.method == DELETE) {
		decision.handler_type = DELETE_RESOURCE;
		return ERR_NONE;
	}
	if (request.method == POST) {
		decision.handler_type = UPLOAD;
		return ERR_NONE;
	}
	if (decision.path_type == directory) {
		decision.handler_type = DIRECTORY;
		return ERR_NONE;
	}
	decision.handler_type = STATIC_FILE;
	return ERR_NONE;
}

}
}
