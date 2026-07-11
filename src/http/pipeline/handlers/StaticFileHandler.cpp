
#include "http/pipeline/handlers/StaticFileHandler.hpp"
#include "http/Context.hpp"

#include <ctime>
#include <sys/stat.h>

namespace http {

namespace {

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
	{".glb", "model/gltf-binary"} // example mime
};

static std::string extensionOf(const std::string& path) {
	std::string::size_type dot = path.find_last_of('.');

	if (dot == std::string::npos)
		return "";
	return path.substr(dot);
}

static const char* contentType(const std::string& path) {
	std::string ext = extensionOf(path);
	usize i = 0;

	while (i < sizeof(g_mime_types) / sizeof(g_mime_types[0])) {
		if (ext == g_mime_types[i].extension)
			return g_mime_types[i].type;
		++i;
	}
	return "application/octet-stream";
}

static std::string httpDate(time_t value) {
	char buffer[64];
	struct tm* time_info = gmtime(&value);

	if (time_info == NULL)
		return "";
	if (strftime(buffer, sizeof(buffer),
		"%a, %d %b %Y %H:%M:%S GMT", time_info) == 0)
		return "";
	return buffer;
}

}

StaticFileHandler::StaticFileHandler(Context& context)
	: RequestHandler(context) {}

StaticFileHandler::~StaticFileHandler() {}

Error StaticFileHandler::handle() {
	struct stat info;
	Error err;

	if (decision().path_type == not_found)
		return ERR_NOT_FOUND;
	if (decision().path_type != file && decision().path_type != executable)
		return ERR_FORBIDDEN;
	if (stat(decision().filesystem_path.c_str(), &info) != 0)
		return ERR_NOT_FOUND;
	TRY(setBodyFile(decision().filesystem_path), err);
	setStatus(OK);
	setContentType(contentType(decision().filesystem_path));
	setHeader("Last-Modified", httpDate(info.st_mtime));
	setContentLength(static_cast<usize>(info.st_size));
	setConnection();
	setDate();
	return ERR_NONE;
}

}
