#pragma once

#include "config/Config.hpp"
#include "http/Error.hpp"
#include "http/Request.hpp"
#include "http/routing/Routing.hpp"

#include <string>
#include <vector>

namespace http {

class Context;

class CStringArray {
private:
	std::vector<std::string> storage_;
	std::vector<char*> values_;

	CStringArray(const CStringArray&);
	CStringArray& operator=(const CStringArray&);

public:
	CStringArray();

	void clear();
	void add(const std::string& value);
	void add(const std::string& key, const std::string& value);
	char* const* data();
	usize size() const;
};

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

struct CGIExecContext {
	std::string working_dir;
	int         stdin_fd;

	CStringArray argv;
	CStringArray envp;
};

Error buildCGIContext(const Request& request,
	const DispatchInfo& decision,
	CGIRequestContext& request_ctx,
	CGIExecContext& exec_ctx);

Error buildCGIContext(const Context& context,
	CGIRequestContext& request_ctx,
	CGIExecContext& exec_ctx);

}
