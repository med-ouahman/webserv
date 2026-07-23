#include "http/routing/RoutingInternal.hpp"

#include <cerrno>
#include <cstdlib>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace http {
namespace routing {

static bool isHex(char c) {
	return (c >= '0' && c <= '9')
		|| (c >= 'a' && c <= 'f')
		|| (c >= 'A' && c <= 'F');
}

static char hexValue(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return c - 'A' + 10;
}

static Error decodePercent(const std::string& path, std::string& out) {

	usize i = 0;
	char decoded;

	out.clear();
	while (i < path.size()) {
		if (path[i] != '%') {
			out += path[i++];
			continue;
		}
		if (i + 2 >= path.size() || !isHex(path[i + 1])
			|| !isHex(path[i + 2]))
			return ERR_BAD_REQUEST;
		decoded = static_cast<char>((hexValue(path[i + 1]) << 4) | hexValue(path[i + 2]));
		if (decoded == '\0')
			return ERR_BAD_REQUEST;
		out += decoded;
		i += 3;
	}
	return ERR_NONE;
}

static Error trimPath(const std::string& path, std::string& out) {
	std::vector<std::string> parts;
	usize start = 0;
	usize end;
	std::string part;
	usize i;

	if (path.empty() || path[0] != '/')
		return ERR_BAD_REQUEST;
	while (start < path.size()) {
		while (start < path.size() && path[start] == '/')
			++start;
		end = start;
		while (end < path.size() && path[end] != '/')
			++end;
		part = path.substr(start, end - start);
		if (part == "..") {
			if (parts.empty())
				return ERR_FORBIDDEN;
			parts.pop_back();
		}
		else if (!part.empty() && part != ".")
			parts.push_back(part);
		start = end;
	}
	out = "/";
	i = 0;
	while (i < parts.size()) {
		if (i != 0)
			out += "/";
		out += parts[i];
		++i;
	}
	return ERR_NONE;
}

static std::string pathJoin(const std::string& root,
		const std::string& path) {
	if (root.empty())
		return path;
	if (root[root.size() - 1] == '/')
		return path == "/" ? root : root + path.substr(1);
	return root + "/" + path.substr(1);
}

static std::string applyPathLocation(const std::string& path,
		const std::string& location) {
	if (location == "/" || location.empty())
		return path;
	if (path.size() == location.size())
		return "/";
	return path.substr(location.size());
}

static bool rootPrefix(const std::string& root,
		const std::string& path) {
	if (path.compare(0, root.size(), root) != 0)
		return false;
	return path.size() == root.size() || path[root.size()] == '/';
}

static Error checkSymlinkEscape(const std::string& root,
		const std::string& path) {
	char real_root[PATH_MAX];
	char real_path[PATH_MAX];

	if (realpath(root.c_str(), real_root) == NULL)
		return ERR_NOT_FOUND;
	if (realpath(path.c_str(), real_path) == NULL)
		return ERR_NONE;
	if (!rootPrefix(real_root, real_path))
		return ERR_FORBIDDEN;
	return ERR_NONE;
}

Error pathNormalize(const std::string& path, std::string& out) {
	std::string decoded;
	Error err;

	err = decodePercent(path, decoded);
	if (err != ERR_NONE)
		return err;
	return trimPath(decoded, out);
}

Error fsBuildPath(const config::ServerConfig& server,
		const config::LocationConfig& location,
		const std::string& normalized_path,
		std::string& out) {
	const std::string& root = location.root.empty()
		? server.root : location.root;

	if (root.empty())
		return ERR_NOT_FOUND;
	out = pathJoin(root, applyPathLocation(normalized_path, location.path));
	return ERR_NONE;
}

Error fsInspectPath(const std::string& root,
		const std::string& path,
		PathType& type) {
	struct stat info;
	Error err;

	err = checkSymlinkEscape(root, path);
	if (err != ERR_NONE)
		return err;
	if (stat(path.c_str(), &info) != 0) {
		if (errno == ENOENT || errno == ENOTDIR) {
			type = not_found;
			return ERR_NONE;
		}
		return ERR_FORBIDDEN;
	}
	if (S_ISDIR(info.st_mode))
		type = directory;
	else if (S_ISREG(info.st_mode) && access(path.c_str(), X_OK) == 0)
		type = executable;
	else if (S_ISREG(info.st_mode))
		type = file;
	else
		type = not_found;
	return ERR_NONE;
}

}
}
