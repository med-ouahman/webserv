
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/common/Status.hpp"
#include "http/Context.hpp"
#include "http/pipeline/ARequestHandler.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>

namespace http {

namespace {

static bool readErrorPage(const config::ServerConfig& server,
		StatusCode status, std::string& body) {
	std::map<int, std::string>::const_iterator it =
		server.error_pages.find(static_cast<int>(status));
	std::ifstream file;
	std::ostringstream out;

	if (it == server.error_pages.end())
		return false;
	file.open(it->second.c_str(), std::ios::in | std::ios::binary);
	if (!file)
		return false;
	out << file.rdbuf();
	if (file.bad())
		return false;
	body = out.str();
	return true;
}

}

ErrorHandler::ErrorHandler(Context& context, Error error)
	: ARequestHandler(context), error_(error) {}

void ErrorHandler::setAllowedMethods() {
	std::set<std::string>::const_iterator it;
	std::ostringstream methods;
	bool first = true;

	if (!context_.info.dispatch.has_value()
		|| context_.info.dispatch.value.location == NULL)
		return ;
	it = context_.info.dispatch.value.location->allowed_methods.begin();
	while (it != context_.info.dispatch.value.location->allowed_methods.end()) {
		if (!first)
			methods << ", ";
		methods << *it;
		first = false;
		++it;
	}
	setHeader("Allow", methods.str());
}

Error ErrorHandler::handle() {
	std::ostringstream body;
	std::string page;
	StatusCode status;

	status = statusFromError(error_);
	setStatus(status);
	if (status == METHOD_NOT_ALLOWED)
		setAllowedMethods();
	if (!context_.info.dispatch.has_value()
		|| context_.info.dispatch.value.server == NULL
		|| !readErrorPage(*context_.info.dispatch.value.server,
			status, page)) {
		body << "<!doctype html><html><body><h1>"
			<< static_cast<int>(status) << " "
			<< statusMessage(status)
			<< "</h1></body></html>\n";
		setBodyFixed(body.str());
	}
	else
		setBodyFixed(page);
	setContentType("text/html");
	setContentLength();
	setHeader("Connection", "close");
	setDate();

	responseReady();

	return ERR_NONE;
}

StatusCode ErrorHandler::statusFromError(Error error) {
	switch (error) {
		case ERR_NONE:
			return OK;
		case ERR_BAD_REQUEST:
		case ERR_MISSING_HOST:
		case ERR_DUPLICATE_HEADER:
		case ERR_INVALID_CONTENT_LENGTH:
		case ERR_CONFLICTING_BODY_HEADERS:
			return BAD_REQUEST;
		case ERR_UNAUTHORIZED:
			return UNAUTHORIZED;
		case ERR_UNSUPPORTED_HTTP_VERSION:
			return HTTP_VERSION_NOT_SUPPORTED;
		case ERR_TE_UNSUPPORTED:
			return NOT_IMPLEMENTED;
		case ERR_HEADER_TOO_LARGE:
		case ERR_BODY_TOO_LARGE:
			return PAYLOAD_TOO_LARGE;
		case ERR_REQUEST_TIMEOUT:
			return REQUEST_TIMEOUT;
		case ERR_CGI_TIMEOUT:
			return GATEWAY_TIMEOUT;
		case ERR_NOT_FOUND:
			return NOT_FOUND;
		case ERR_FORBIDDEN:
			return FORBIDDEN;
		case ERR_METHOD_NOT_ALLOWED:
			return METHOD_NOT_ALLOWED;
		case ERR_CONFLICT:
			return CONFLICT;
		case ERR_LENGTH_REQUIRED:
			return LENGTH_REQUIRED;
		case ERR_BAD_GATEWAY:
			return BAD_GATEWAY;
		case ERR_INTERNAL:
			return INTERNAL_SERVER_ERROR;
	}
	return INTERNAL_SERVER_ERROR;
}

}
