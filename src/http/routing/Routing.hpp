
#pragma once

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Error.hpp"
#include "http/Request.hpp"

namespace http {

enum PathType { file, directory, executable, not_found };

enum RequestType {
	STATIC_FILE,
	DIRECTORY,
	UPLOAD,
	CGI,
	REDIRECT,
	DELETE_RESOURCE,
};


struct DispatchInfo {
	const config::ServerConfig* server;
	const config::LocationConfig* location;
	std::string upload_path;
	const std::string* cgi_path;
	std::string normalized_path;
	std::string filesystem_path;
	PathType path_type;
	usize max_body_size;
	size_t cgi_timeout;

	RequestType handler_type;
	bool read_body;

	DispatchInfo();
};

base::Expected<DispatchInfo, Error> route(const Request& request,
	const std::vector<const config::ServerConfig*>& servers,
	DispatchInfo* partial = NULL);

}
