
#include "http/pipeline/ARequestHandler.hpp"
#include "http/Context.hpp"
#include "server/ServerInfo.hpp"
#include <cerrno>
#include <sstream>
#include <ctime>

namespace http {

namespace {

struct MimeEntry {
	const char* extension;
	const char* type;
};

static const MimeEntry g_mime_types[] = {
	{ ".html", "text/html" },
	{ ".htm", "text/html" },
	{ ".css", "text/css" },
	{ ".js", "application/javascript" },
	{ ".json", "application/json" },
	{ ".png", "image/png" },
	{ ".jpg", "image/jpeg" },
	{ ".jpeg", "image/jpeg" },
	{ ".gif", "image/gif" },
	{ ".svg", "image/svg+xml" },
	{ ".txt", "text/plain" },
	{ ".ico", "image/x-icon" },
	{ ".glb", "model/gltf-binary" }
};

static std::string extensionOf(const std::string& path) {
	std::string::size_type dot = path.find_last_of('.');

	if (dot == std::string::npos)
		return "";
	return path.substr(dot);
}

static const char* contentTypeFromPath(const std::string& path) {
	std::string ext = extensionOf(path);
	usize i = 0;

	while (i < sizeof(g_mime_types) / sizeof(g_mime_types[0])) {
		if (ext == g_mime_types[i].extension)
			return g_mime_types[i].type;
		++i;
	}
	return "application/octet-stream";
}

}

ARequestHandler::ARequestHandler(Context& context)
	: context_(context) {}

ARequestHandler::~ARequestHandler() {}

Error ARequestHandler::timeout() { return ERR_NONE; }

void ARequestHandler::monitor() { }

void ARequestHandler::setStatus(StatusCode status) {
	context_.actor.response.status = status;
}

void ARequestHandler::setHeader(const std::string& key,
		const std::string& value) {
	usize i = 0;
	Header header;

	while (i < context_.actor.response.headers.size()) {
		if (context_.actor.response.headers[i].key == key) {
			context_.actor.response.headers[i].value = value;
			return ;
		}
		++i;
	}
	header.key = key;
	header.value = value;
	context_.actor.response.headers.push_back(header);
}

void ARequestHandler::eraseHeader(const std::string& key) {
	std::vector<Header>::iterator it = context_.actor.response.headers.begin();

	while (it != context_.actor.response.headers.end()) {
		if (it->key == key)
			it = context_.actor.response.headers.erase(it);
		else
			++it;
	}
}

void ARequestHandler::setBodyFixed(const std::string& body) {
	context_.actor.response.body = body;
	context_.actor.response.body_reader.reset();
}

Error ARequestHandler::fileAccessError() const {
	if (errno == ENOENT || errno == ENOTDIR)
		return ERR_NOT_FOUND;
	if (errno == EACCES || errno == EPERM)
		return ERR_FORBIDDEN;
	return ERR_INTERNAL;
}

Error ARequestHandler::setBodyFile(const std::string& path) {
	context_.actor.response.body.clear();
	if (!context_.actor.response.body_reader.reset(path))
		return fileAccessError();
	return ERR_NONE;
}

void ARequestHandler::setContentType(const std::string& type) {
	setHeader("Content-Type", type);
}

void ARequestHandler::setContentTypeFromPath(const std::string& path) {
	setContentType(contentTypeFromPath(path));
}

void ARequestHandler::setContentLength() {
	setContentLength(context_.actor.response.body.size());
}

void ARequestHandler::setContentLength(usize size) {
	
	setHeader("Content-Length", base::to_string(size));
}

void ARequestHandler::setConnection() {
	if (context_.actor.request.version == HTTP_1_1
		&& context_.actor.request.connection != CONNECTION_CLOSE) {
		setHeader("Connection", "keep-alive");
		return ;
	}
	if (context_.actor.request.version == HTTP_1_0
		&& context_.actor.request.connection == CONNECTION_KEEP_ALIVE) {
		setHeader("Connection", "keep-alive");
		return ;
	}
	setHeader("Connection", "close");
}

void ARequestHandler::setDate() {
	char buffer[64];
	time_t now = time(NULL);
	struct tm* time_info = gmtime(&now);

	if (time_info == NULL)
		return ;
	if (strftime(buffer, sizeof(buffer),
		"%a, %d %b %Y %H:%M:%S GMT", time_info) == 0)
		return ;
	setHeader("Date", buffer);
}

void ARequestHandler::setCookieHeader(const std::string& sid,
	const std::string& path,
	const std::string& additional) {
	
	setHeader("Set-Cookie", sid+"; "+path+"; "+additional);
}

void ARequestHandler::setServerHeader() {
	setHeader("Server", server_info::info());	
}

void ARequestHandler::responseReady() {
	setServerHeader();
	context_.responseReady();
}


const DispatchInfo& ARequestHandler::decision() const {
	return context_.info.dispatch.value;
}

Request& ARequestHandler::request() { return context_.actor.request; }

Response& ARequestHandler::response() { return context_.actor.response; }

}
