
#include "http/pipeline/handlers/StaticFileHandler.hpp"
#include "http/Context.hpp"

#include <ctime>
#include <sys/stat.h>

namespace http {

namespace {

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
	: ARequestHandler(context) {}

StaticFileHandler::~StaticFileHandler() {}

Error StaticFileHandler::handle() {
	struct stat info;
	Error err;

	if (decision().path_type == not_found)
		return ERR_NOT_FOUND;
	if (decision().path_type != file && decision().path_type != executable)
		return ERR_FORBIDDEN;
	if (stat(decision().filesystem_path.c_str(), &info) != 0)
		return fileAccessError();
	TRY(setBodyFile(decision().filesystem_path), err);
	setStatus(OK);
	setContentTypeFromPath(decision().filesystem_path);
	setHeader("Last-Modified", httpDate(info.st_mtime));
	setContentLength(static_cast<usize>(info.st_size));
	setConnection();
	setDate();
	responseReady();
	return ERR_NONE;
}

}
