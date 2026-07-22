
#include "http/pipeline/handlers/DirectoryHandler.hpp"
#include "http/Context.hpp"
#include "config/Config.hpp"

#include <dirent.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace http {

namespace {

static std::string pathJoin(const std::string& left,
		const std::string& right) {
	if (left.empty() || left[left.size() - 1] == '/')
		return left + right;
	return left + "/" + right;
}

static bool regularFile(const std::string& path) {
	struct stat info;

	return stat(path.c_str(), &info) == 0 && S_ISREG(info.st_mode);
}

static Error readFile(const std::string& path, std::string& out) {
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);
	std::ostringstream body;

	if (!file)
		return ERR_NOT_FOUND;
	body << file.rdbuf();
	if (file.bad())
		return ERR_INTERNAL;
	out = body.str();
	return ERR_NONE;
}

static std::string htmlEscape(const std::string& value) {
	std::string out;
	usize i = 0;

	while (i < value.size()) {
		if (value[i] == '&')
			out += "&amp;";
		else if (value[i] == '<')
			out += "&lt;";
		else if (value[i] == '>')
			out += "&gt;";
		else if (value[i] == '"')
			out += "&quot;";
		else
			out += value[i];
		++i;
	}
	return out;
}

static Error buildAutoindex(const std::string& fs_path,
		const std::string& uri_path,
		std::string& out) {
	DIR* dir = opendir(fs_path.c_str());
	struct dirent* entry;
	std::ostringstream body;
	std::string base = uri_path;

	if (dir == NULL)
		return ERR_FORBIDDEN;
	if (base.empty() || base[base.size() - 1] != '/')
		base += "/";
	body << "<!doctype html><html><head><meta charset=\"utf-8\">"
		<< "<title>Index of " << htmlEscape(uri_path) << "</title>"
		<< "</head><body><h1>Index of " << htmlEscape(uri_path)
		<< "</h1><ul>";
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name == ".")
			continue;
		body << "<li><a href=\"" << htmlEscape(base + name)
			<< "\">" << htmlEscape(name) << "</a></li>";
	}
	closedir(dir);
	body << "</ul></body></html>";
	out = body.str();
	return ERR_NONE;
}

static const std::vector<std::string>& indexFiles(
		const config::LocationConfig& location,
		const config::ServerConfig& server) {
	if (!location.index.empty())
		return location.index;
	return server.index_files;
}

}

DirectoryHandler::DirectoryHandler(Context& context)
	: ARequestHandler(context) {}

DirectoryHandler::~DirectoryHandler() {}

Error DirectoryHandler::handle() {
	const config::ServerConfig& server = *decision().server;
	const std::vector<std::string>& indexes =
		indexFiles(*decision().location, server);
	std::string body;
	Error err;
	usize i;

	i = 0;
	while (i < indexes.size()) {
		std::string path = pathJoin(decision().filesystem_path, indexes[i]);
		if (regularFile(path)) {
			TRY(readFile(path, body), err);
			setStatus(OK);
			setBodyFixed(body);
			setContentType("text/html");
			setContentLength();
			setConnection();
			setDate();
			responseReady();
			return ERR_NONE;
		}
		i++;
	}
	if (!decision().location->autoindex)
		return ERR_FORBIDDEN;
	TRY(buildAutoindex(decision().filesystem_path,
		decision().normalized_path, body), err);
	setStatus(OK);
	setBodyFixed(body);
	setContentType("text/html");
	setContentLength();
	setConnection();
	setDate();
	responseReady();
	return ERR_NONE;
}

}
