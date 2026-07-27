#pragma once

#include "UniqueFd.hpp"
#include <unistd.h>
#include "config/Config.hpp"
#include "http/Error.hpp"
#include "CStringArray.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace cgi {

struct CGIRequestContext {
	std::string request_method;
	std::string mime_type;
	std::string interpreter;
	std::string script_name;
	std::string script_filename;
	std::string query_string;
	std::string content_length;
	std::string path_info;
	std::string server_name;
	std::string server_protocol;
	std::string server_port;
	size_t		timeout;
};

struct ProcessContext {
	std::string working_dir;
	mutable UniqueFd	stdin_fd;

	CStringArray argv;
	CStringArray envp;

	~ProcessContext() {
		argv.clear();
		envp.clear();
	}

	ProcessContext()
	: stdin_fd(-1) {}
};

http::Error buildCGIContext(const http::Context& context,
	CGIRequestContext& request_ctx,
	ProcessContext& exec_ctx);

}
