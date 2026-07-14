
#pragma once

#include "base/base.hpp"
#include "config/Config.hpp"
#include "http/Error.hpp"
#include "http/Request.hpp"

namespace http {

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
	const std::string* upload_path;
	const std::string* cgi_path;
	std::string normalized_path;
	std::string filesystem_path;
	PathType path_type;
	usize max_body_size;

	RequestType handlerType;
	bool read_body;

	DispatchInfo();
};

base::Expected<DispatchInfo, Error> route(const Request& request,
	const config::Config& config, DispatchInfo* partial = NULL);

base::Expected<DispatchInfo, Error> route(const Request& request,
	const config::ServerConfig& server, DispatchInfo* partial = NULL);

base::Expected<DispatchInfo, Error> route(const Request& request,
	const std::vector<const config::ServerConfig*>& servers,
	DispatchInfo* partial = NULL);

Error decide(const Request& request,
	const config::Config& config,
	base::Optional<DispatchInfo>& dispatch_info);

Error decide(const Request& request,
	const config::ServerConfig& server,
	base::Optional<DispatchInfo>& dispatch_info);

Error decide(const Request& request,
	const std::vector<const config::ServerConfig*>& servers,
	base::Optional<DispatchInfo>& dispatch_info);

}
