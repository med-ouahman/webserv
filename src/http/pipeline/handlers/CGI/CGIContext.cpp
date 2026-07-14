#include "http/pipeline/handlers/CGI/CGIContext.hpp"
#include "http/Context.hpp"

#include <cctype>
#include <sstream>

namespace http {

namespace {

static std::string toString(usize value) {
	std::ostringstream out;

	out << value;
	return out.str();
}

static const char* methodName(Method method) {
	switch (method) {
		case GET:
			return "GET";
		case POST:
			return "POST";
		case DELETE:
			return "DELETE";
		case UNKNOWN:
			return "";
	}
	return "";
}

static const char* versionName(Version version) {
	switch (version) {
		case HTTP_1_0:
			return "HTTP/1.0";
		case HTTP_1_1:
			return "HTTP/1.1";
		case HTTP_UNKNOWN:
			return "HTTP/1.1";
	}
	return "HTTP/1.1";
}

static char lowerChar(char c) {
	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

static std::string lowerName(const std::string& value) {
	std::string out = value;
	usize i = 0;

	while (i < out.size()) {
		out[i] = lowerChar(out[i]);
		++i;
	}
	return out;
}

static const Header* findHeader(const Request& request,
		const std::string& name) {
	usize i = 0;

	while (i < request.headers.size()) {
		if (lowerName(request.headers[i].key) == name)
			return &request.headers[i];
		++i;
	}
	return NULL;
}

static bool skipHTTPHeader(const std::string& name) {
	return name == "content-type"
		|| name == "content-length"
		|| name == "connection"
		|| name == "transfer-encoding";
}

static std::string envHeaderName(const std::string& name) {
	std::string out = "HTTP_";
	usize i = 0;

	while (i < name.size()) {
		if (name[i] == '-')
			out += '_';
		else
			out += static_cast<char>(
				std::toupper(static_cast<unsigned char>(name[i])));
		++i;
	}
	return out;
}

static std::string dirnameOf(const std::string& path) {
	std::string::size_type slash = path.find_last_of('/');

	if (slash == std::string::npos || slash == 0)
		return "/";
	return path.substr(0, slash);
}

static std::string serverName(const Request& request,
		const config::ServerConfig& server) {
	if (request.host.has_value())
		return request.host.value;
	if (!server.server_names.empty())
		return server.server_names[0];
	return "localhost";
}

static std::string serverPort(const config::ServerConfig& server) {
	if (server.listens.empty())
		return "";
	return toString(server.listens[0].port);
}

static void fillRequestContext(const Request& request,
		const DispatchInfo& decision,
		const config::Config& config,
		CGIRequestContext& out) {
	const Header* content_type = findHeader(request, "content-type");

	out.request_method = methodName(request.method);
	out.mime_type = content_type == NULL ? "" : content_type->value;
	out.interpreter = decision.cgi_path == NULL ? "" : *decision.cgi_path;
	out.script_name = decision.normalized_path;
	out.query_string = request.query.has_value() ? request.query.value : "";
	out.content_length = request.content_length.has_value()
		? toString(request.content_length.value) : "";
	out.path_info = "";
	out.server_name = serverName(request, config.server);
	out.server_protocol = versionName(request.version);
	out.server_port = serverPort(config.server);
}

static void fillEnv(const Request& request,
		const CGIRequestContext& request_ctx,
		CGIExecContext& exec_ctx) {
	usize i = 0;

	exec_ctx.envp.add("REQUEST_METHOD", request_ctx.request_method);
	exec_ctx.envp.add("SERVER_PROTOCOL", request_ctx.server_protocol);
	exec_ctx.envp.add("QUERY_STRING", request_ctx.query_string);
	exec_ctx.envp.add("CONTENT_TYPE", request_ctx.mime_type);
	exec_ctx.envp.add("CONTENT_LENGTH", request_ctx.content_length);
	exec_ctx.envp.add("GATEWAY_INTERFACE", "CGI/1.1");
	exec_ctx.envp.add("SCRIPT_NAME", request_ctx.script_name);
	exec_ctx.envp.add("PATH_INFO", request_ctx.path_info);
	exec_ctx.envp.add("SERVER_NAME", request_ctx.server_name);
	exec_ctx.envp.add("SERVER_PORT", request_ctx.server_port);
	while (i < request.headers.size()) {
		std::string normalized = lowerName(request.headers[i].key);

		if (!skipHTTPHeader(normalized))
			exec_ctx.envp.add(envHeaderName(request.headers[i].key),
				request.headers[i].value);
		++i;
	}
}

}

CStringArray::CStringArray()
	: storage_(),
	  values_() {}

void CStringArray::clear() {
	storage_.clear();
	values_.clear();
}

void CStringArray::add(const std::string& value) {
	storage_.push_back(value);
}

void CStringArray::add(const std::string& key, const std::string& value) {
	storage_.push_back(key + "=" + value);
}

char* const* CStringArray::data() {
	usize i = 0;

	values_.clear();
	values_.reserve(storage_.size() + 1);
	while (i < storage_.size()) {
		values_.push_back(const_cast<char*>(storage_[i].c_str()));
		++i;
	}
	values_.push_back(NULL);
	return &values_[0];
}

usize CStringArray::size() const {
	return storage_.size();
}

Error buildCGIContext(const Request& request,
		const DispatchInfo& decision,
		const config::Config& config,
		CGIRequestContext& request_ctx,
		CGIExecContext& exec_ctx) {
	if (decision.cgi_path == NULL || decision.cgi_path->empty())
		return ERR_INTERNAL;
	if (decision.filesystem_path.empty())
		return ERR_NOT_FOUND;
	request_ctx = CGIRequestContext();
	exec_ctx.working_dir = decision.location != NULL
		&& !decision.location->cgi_dir.empty()
		? decision.location->cgi_dir : dirnameOf(decision.filesystem_path);
	exec_ctx.stdin_fd = -1;
	exec_ctx.argv.clear();
	exec_ctx.envp.clear();
	fillRequestContext(request, decision, config, request_ctx);
	exec_ctx.argv.add(request_ctx.interpreter);
	exec_ctx.argv.add(decision.filesystem_path);
	fillEnv(request, request_ctx, exec_ctx);
	return ERR_NONE;
}

Error buildCGIContext(const Context& context,
		const config::Config& config,
		CGIRequestContext& request_ctx,
		CGIExecContext& exec_ctx) {
	if (!context.info.dispatch.has_value())
		return ERR_INTERNAL;
	return buildCGIContext(context.actor.request, context.info.dispatch.value, config,
		request_ctx, exec_ctx);
}

}
