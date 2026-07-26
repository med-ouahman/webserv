
#include "http/pipeline/handlers/DirectoryHandler.hpp"
#include "http/Context.hpp"
#include "config/Config.hpp"

#include <dirent.h>
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

static bool regularFile(struct stat& info) {
	return S_ISREG(info.st_mode);
}

static bool exists(const std::string& path, struct stat& info) {

	return stat(path.c_str(), &info) == 0;
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
		struct stat info;
		std::cout << "THE PATH: " << path << "\n";

		/*
			IF EXISTS 
		*/

		if (!exists(path, info)) return ERR_NOT_FOUND;

		if (regularFile(info)) {
			TRY(setBodyFile(path), err);
			setStatus(OK);
			setContentTypeFromPath(path);
			setContentLength(static_cast<usize>(info.st_size));
			setConnection();
			setDate();
			responseReady();
			return ERR_NONE;
		}
		i++;
	}
	if (!decision().location->autoindex)
	{
		std::cout << "WHEN THE SUN RISES, YOU WILL SEE THE TRUTH IN THE EYES OF PEOPLE WHOM YOU WRONGED\n";
		return ERR_FORBIDDEN;
	}
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
