
#include "http/routing/RoutingInternal.hpp"
#include <sys/stat.h>

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

static std::string pathJoin(const std::string& left,
		const std::string& right) {
	if (left.empty() || left[left.size() - 1] == '/')
		return left + right;
	return left + "/" + right;
}

static std::string effectiveRoot(const DispatchInfo& decision) {
	if (!decision.location->root.empty())
		return decision.location->root;
	return decision.server->root;
}

static bool pathExists(const std::string& path) {
	struct stat info;

	return stat(path.c_str(), &info) == 0;
}

static bool rootPrefixed(const std::string& root,
		const std::string& configured) {
	if (root.empty())
		return false;
	if (configured.compare(0, root.size(), root) == 0)
		return true;
	if (configured.size() > 2 && configured[0] == '.' && configured[1] == '/'
		&& root.size() > 2 && root[0] == '.' && root[1] == '/'
		&& configured.compare(2, root.size() - 2, root.substr(2)) == 0)
		return true;
	return false;
}

static std::string relativeUploadPath(const std::string& configured) {
	if (configured.size() > 2 && configured[0] == '.' && configured[1] == '/')
		return configured.substr(2);
	return configured;
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

Error resolveUploadPath(DispatchInfo& decision) {
	const std::string& configured = decision.location->upload_path;

	if (configured.empty())
		return ERR_FORBIDDEN;
	if (configured[0] == '/' || pathExists(configured)
		|| rootPrefixed(effectiveRoot(decision), configured)) {
		decision.upload_path = configured;
		return ERR_NONE;
	}
	decision.upload_path = pathJoin(effectiveRoot(decision),
		relativeUploadPath(configured));
	return ERR_NONE;
}

Error setRequestType(const Request& request,
		DispatchInfo& decision) {
	decision.cgi_path = findCgiPath(decision);

	if (decision.location->path == "/counter") {
		decision.handler_type = COUNTER;
		return ERR_NONE;
	}

	if (decision.location->path == "/login") {
		decision.handler_type = LOGIN;
		return ERR_NONE;
	}
	/* add logout to flex */

	if (decision.location->path == "/profile") {
		decision.handler_type = PROFILE;
		return ERR_NONE;
	}

	if (decision.location->path == "/logout") {
		decision.handler_type = LOGOUT;
		return ERR_NONE;
	}

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
