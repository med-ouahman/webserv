
#include "cgi/CGIContext.hpp"
#include "http/Context.hpp"
#include <fcntl.h>
#include <cctype>
#include <sstream>
#include <unistd.h>
#include <cstdlib>

namespace cgi {

namespace {

static std::string toString(usize value) {
	std::ostringstream out;

	out << value;
	return out.str();
}

static std::string absolute_path(const std::string& path) {

	char* p = ::realpath(path.c_str(), NULL);

	if (!p) return "";

	std::string out = p;

	::free(p);
	return out;
}

static const char* methodName(http::Method method) {
	switch (method) {
		case http::GET:
			return "GET";
		case http::POST:
			return "POST";
		case http::DELETE:
			return "DELETE";
		case http::UNKNOWN:
			return "";
	}
	return "";
}

static const char* versionName(http::Version version) {
	switch (version) {
		case http::HTTP_1_0:
			return "HTTP/1.0";
		case http::HTTP_1_1:
			return "HTTP/1.1";
		case http::HTTP_UNKNOWN:
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

static const http::Header* findHeader(const http::Request& request,
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

static std::string serverName(const http::Request& request,
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

static void fillRequestContext(const http::Request& request,
		const http::DispatchInfo& decision,
		CGIRequestContext& out) {
	const http::Header* content_type = findHeader(request, "content-type");

	out.request_method = methodName(request.method);
	out.mime_type = content_type == NULL ? "" : content_type->value;
	out.interpreter = decision.cgi_path == NULL ? "" : *decision.cgi_path;
	out.script_name = decision.normalized_path;
	out.query_string = request.query.has_value() ? request.query.value : "";
	out.content_length = request.content_length.has_value()
		? toString(request.content_length.value) : "";
	out.path_info = "";
	out.server_name = serverName(request, *decision.server);
	out.server_protocol = versionName(request.version);
	out.server_port = serverPort(*decision.server);
	out.timeout = decision.cgi_timeout;
	out.script_filename = absolute_path(decision.filesystem_path);
}

static void fillEnv(const http::Request& request,
		const CGIRequestContext& request_ctx,
		ProcessContext& exec_ctx) {
	usize i = 0;

	exec_ctx.envp.push("REQUEST_METHOD=" + request_ctx.request_method);
	exec_ctx.envp.push("REQUEST_URI=" + request.url);
	exec_ctx.envp.push("SERVER_PROTOCOL=" + request_ctx.server_protocol);
	exec_ctx.envp.push("GATEWAY_INTERFACE=CGI/1.1");
	exec_ctx.envp.push("QUERY_STRING=" + request_ctx.query_string);
	exec_ctx.envp.push("CONTENT_TYPE=" + request_ctx.mime_type);
	exec_ctx.envp.push("CONTENT_LENGTH=" + request_ctx.content_length);
	
	exec_ctx.envp.push("SERVER_NAME=" + request_ctx.server_name);
	exec_ctx.envp.push("SERVER_PORT=" + request_ctx.server_port);
	exec_ctx.envp.push("REDIRECT_STATUS=200");

	exec_ctx.envp.push("SCRIPT_FILENAME="+request_ctx.script_filename);
	exec_ctx.envp.push("PATH_INFO="+request_ctx.script_name);

	while (i < request.headers.size()) {
		std::string normalized = lowerName(request.headers[i].key);

		if (!skipHTTPHeader(normalized))
			exec_ctx.envp.push(envHeaderName(request.headers[i].key)+"="
				+request.headers[i].value);
		++i;
	}

	usize size = 0;
	for ( ; __environ[size]; ++size );
	
	exec_ctx.envp.push_array(const_cast<const char**>(__environ), size);
	(void)request;
}

static http::Error setStdin(const http::Request& request,
		ProcessContext& exec_ctx) {

	if (base::io::Reader::FILE == request.body.type()) {
		exec_ctx.stdin_fd.reset(::open(request.body.path().c_str(), O_RDONLY));
		
		if (!exec_ctx.stdin_fd.valid()) return http::ERR_INTERNAL;

	} else {
		exec_ctx.stdin_fd.reset(STDIN_FILENO);
	}
	
	return http::ERR_NONE;
}

static void setArgv(const http::DispatchInfo& decision,
		const CGIRequestContext& request_ctx,
		ProcessContext& exec_ctx) {
	if (decision.cgi_path != NULL && !decision.cgi_path->empty()) {
		exec_ctx.argv.push(absolute_path(request_ctx.interpreter));
		exec_ctx.argv.push(decision.filesystem_path);
	}
	else exec_ctx.argv.push(decision.filesystem_path);
}

static http::Error buildProcessContext(const http::Request& request,
		const http::DispatchInfo& decision,
		const CGIRequestContext& request_ctx,
		ProcessContext& exec_ctx) {
	http::Error err;

	exec_ctx.working_dir = absolute_path(dirnameOf(decision.filesystem_path));
	TRY(setStdin(request, exec_ctx), err);
	setArgv(decision, request_ctx, exec_ctx);
	fillEnv(request, request_ctx, exec_ctx);
	return http::ERR_NONE;
}

}

http::Error buildCGIContext(const http::Context& context,
			CGIRequestContext& request_ctx,
			ProcessContext& exec_ctx) {
	const http::Request& request = context.actor.request;
	const http::DispatchInfo& decision = context.info.dispatch.value;
	http::Error err;

	request_ctx = CGIRequestContext();
	fillRequestContext(request, decision, request_ctx);
	TRY(buildProcessContext(request, decision, request_ctx, exec_ctx), err);
	return http::ERR_NONE;
}


}
