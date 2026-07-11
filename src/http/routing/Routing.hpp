
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


struct Decision {
	const config::LocationConfig* location;
	const std::string* upload_path;
	const std::string* cgi_path;
	std::string normalized_path;
	std::string filesystem_path;
	PathType path_type;
	usize max_body_size;

	RequestType handlerType;
	bool read_body;

	Decision();
};

base::Expected<Decision, Error> route(const Request& request,
	const config::Config& config);

}
