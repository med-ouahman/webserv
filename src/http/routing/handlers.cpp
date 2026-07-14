#include "http/routing/RoutingInternal.hpp"

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

bool isCGIRequest(const std::string& path,
		const config::LocationConfig& location,
		const std::string** cgi_path) {
	std::map<std::string, std::string>::const_iterator it;

	it = location.cgi_pass.begin();
	while (it != location.cgi_pass.end()) {
		if (extensionMatches(path, it->first)) {
			*cgi_path = &it->second;
			return true;
		}
		++it;
	}
	if (extensionMatches(path, location.cgi_extension)
		&& !location.cgi_path.empty()) {
		*cgi_path = &location.cgi_path;
		return true;
	}
	return false;
}

Error checkUploadAllowed(const config::LocationConfig& location) {
	if (!location.upload_enabled || location.upload_path.empty())
		return ERR_FORBIDDEN;
	return ERR_NONE;
}

Error checkUploadFraming(const Request& request) {
	if (!request.chunked && !request.content_length.has_value())
		return ERR_LENGTH_REQUIRED;
	return ERR_NONE;
}

RequestType getRequestType(const Request& request,
		const std::string& path,
		const config::LocationConfig& location,
		PathType path_type,
		const std::string** cgi_path) {
	if (isCGIRequest(path, location, cgi_path))
		return CGI;
	if (request.method == DELETE)
		return DELETE_RESOURCE;
	if (request.method == POST)
		return UPLOAD;
	if (path_type == directory)
		return DIRECTORY;
	return STATIC_FILE;
}

}
}
