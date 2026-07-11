
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/Context.hpp"
#include "http/pipeline/RequestHandler.hpp"
#include "config/Config.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>

namespace http {

namespace {

static bool readErrorPage(StatusCode status, std::string& body) {
	const config::ServerConfig& server = config::Config::get_config().server;
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
	: RequestHandler(context), error_(error) {}

Error ErrorHandler::handle() {
	std::ostringstream body;
	StatusCode status;

	delete context_.handler_;
	context_.handler_ = NULL;
	status = statusFromError(error_);
	setStatus(status);
	if (!readErrorPage(status, response().body)) {
		body << "<!doctype html><html><body><h1>"
			<< static_cast<int>(status) << " "
			<< statusMsg(status)
			<< "</h1></body></html>";
		setBody(body.str());
	}
	setContentType("text/html");
	setContentLength();
	setConnection();
	setDate();

	context_.responseReady();

	return error_;
}

const char* ErrorHandler::statusMsg(StatusCode code) {
	switch (code) {
		case OK: return "OK";
		case CREATED: return "Created";
		case NO_CONTENT: return "No Content";
		case MOVED_PERMANENTLY: return "Moved Permanently";
		case FOUND: return "Found";
		case SEE_OTHER: return "See Other";
		case TEMPORARY_REDIRECT: return "Temporary Redirect";
		case PERMANENT_REDIRECT: return "Permanent Redirect";
		case BAD_REQUEST: return "Bad Request";
		case FORBIDDEN: return "Forbidden";
		case NOT_FOUND: return "Not Found";
		case METHOD_NOT_ALLOWED: return "Method Not Allowed";
		case REQUEST_TIMEOUT: return "Request Timeout";
		case CONFLICT: return "Conflict";
		case PAYLOAD_TOO_LARGE: return "Payload Too Large";
		case INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case NOT_IMPLEMENTED: return "Not Implemented";
		case BAD_GATEWAY: return "Bad Gateway";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
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
		case ERR_UNSUPPORTED_HTTP_VERSION:
			return HTTP_VERSION_NOT_SUPPORTED;
		case ERR_TE_UNSUPPORTED:
			return NOT_IMPLEMENTED;
		case ERR_HEADER_TOO_LARGE:
		case ERR_BODY_TOO_LARGE:
			return PAYLOAD_TOO_LARGE;
		case ERR_REQUEST_TIMEOUT:
		case ERR_CGI_TIMEOUT:
			return REQUEST_TIMEOUT;
		case ERR_NOT_FOUND:
			return NOT_FOUND;
		case ERR_FORBIDDEN:
			return FORBIDDEN;
		case ERR_METHOD_NOT_ALLOWED:
			return METHOD_NOT_ALLOWED;
		case ERR_CGI_FAILED:
			return BAD_GATEWAY;
		case ERR_INTERNAL:
			return INTERNAL_SERVER_ERROR;
	}
	return INTERNAL_SERVER_ERROR;
}

}
