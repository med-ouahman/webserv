
#include "http/pipeline/handlers/DeleteHandler.hpp"
#include "http/Context.hpp"

#include <cerrno>
#include <cstdio>

namespace http {

namespace {

static Error deleteError() {
	if (errno == ENOENT)
		return ERR_NOT_FOUND;
	if (errno == EACCES || errno == EPERM || errno == EISDIR)
		return ERR_FORBIDDEN;
	return ERR_INTERNAL;
}

static Error deleteFile(const std::string& path) {
	if (std::remove(path.c_str()) != 0)
		return deleteError();
	return ERR_NONE;
}

}

DeleteHandler::DeleteHandler(Context& context)
	: ARequestHandler(context) {}

DeleteHandler::~DeleteHandler() {}

Error DeleteHandler::handle() {
	Error err;

	if (decision().path_type == not_found)
		return ERR_NOT_FOUND;
	if (decision().path_type != file && decision().path_type != executable)
		return ERR_FORBIDDEN;
	TRY(deleteFile(decision().filesystem_path), err);
	setStatus(NO_CONTENT);
	setBodyFixed("");
	setContentLength();
	setConnection();
	setDate();
	done_ = true;
	return ERR_NONE;
}

}
