#include "Status.hpp"

namespace http {

const char* statusMessage(StatusCode code) {
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
		case UNAUTHORIZED: return "Unauthorized";
		case FORBIDDEN: return "Forbidden";
		case NOT_FOUND: return "Not Found";
		case METHOD_NOT_ALLOWED: return "Method Not Allowed";
		case REQUEST_TIMEOUT: return "Request Timeout";
		case CONFLICT: return "Conflict";
		case LENGTH_REQUIRED: return "Length Required";
		case PAYLOAD_TOO_LARGE: return "Payload Too Large";
		case TOO_MANY_REQUESTS: return "Too Many Requests";
		case INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case NOT_IMPLEMENTED: return "Not Implemented";
		case BAD_GATEWAY: return "Bad Gateway";
		case GATEWAY_TIMEOUT: return "Gateway timeout";
		case HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
}

}
