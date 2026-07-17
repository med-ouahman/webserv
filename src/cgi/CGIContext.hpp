#pragma once

#include "config/Config.hpp"
#include "http/Error.hpp"
#include "http/Request.hpp"
#include "http/routing/Routing.hpp"
#include "CStringArray.hpp"

#include <string>
#include <vector>

namespace http {

class Context;

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
};

Error buildCGIContext(const Request& request,
	const DispatchInfo& decision,
	const config::Config& config,
	CGIRequestContext& request_ctx,
	ProcessContext& exec_ctx);

Error buildCGIContext(const Context& context,
	const config::Config& config,
	CGIRequestContext& request_ctx,
	ProcessContext& exec_ctx);

}
