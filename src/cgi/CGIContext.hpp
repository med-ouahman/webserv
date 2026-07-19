#pragma once

#include <unistd.h>
#include "config/Config.hpp"
#include "http/Error.hpp"
#include "http/Request.hpp"
#include "http/routing/Routing.hpp"
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
	int         stdin_fd;

	CStringArray argv;
	CStringArray envp;

	~ProcessContext() {
		argv.clear();
		envp.clear();
		
		if (stdin_fd > 0) {
			::close(stdin_fd);
			stdin_fd = -1;
		}
	}

	ProcessContext()
	:
	stdin_fd(-1) {

	}
};

http::Error buildCGIContext(const http::Request& request,
	const http::DispatchInfo& decision,
	CGIRequestContext& request_ctx,
	ProcessContext& exec_ctx);

http::Error buildCGIContext(const http::Context& context,

	CGIRequestContext& request_ctx,
	ProcessContext& exec_ctx);

}
