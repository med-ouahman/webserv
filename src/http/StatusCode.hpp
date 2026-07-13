
// #pragma once

// #include "http/Error.hpp"

// namespace http {

// /**
// 	* enum StatusCode - HTTP status codes emitted by the server
// 	*
// 	* This list is limited to codes the server is expected to generate in its
// 	* normal request, upload, CGI, and error-handling paths.
//  */
// enum StatusCode {
// 	OK = 200,
// 	CREATED = 201,
// 	NO_CONTENT = 204,

// 	MOVED_PERMANENTLY = 301,

// 	BAD_REQUEST = 400,
// 	FORBIDDEN = 403,
// 	NOT_FOUND = 404,
// 	METHOD_NOT_ALLOWED = 405,
// 	REQUEST_TIMEOUT = 408,
// 	CONFLICT = 409,
// 	PAYLOAD_TOO_LARGE = 413,

// 	INTERNAL_SERVER_ERROR = 500,
// 	NOT_IMPLEMENTED = 501,
// 	BAD_GATEWAY = 502,
// 	GATEWAY_TIMEOUT = 504,
// 	HTTP_VERSION_NOT_SUPPORTED = 505
// };

// /**
// 	* status_msg - return the standard text message for a status code
// 	* @code: status code to translate
// 	*
// 	* Returns the text used in the HTTP status line for @code. Unknown values
// 	* fall back to "Internal Server Error" so response serialization always has
// 	* a non-empty phrase.
// 	* (seriously c++ you couldn't bother adding static reflection ? i guess virtual inheritence too all your budget).
//  */
// inline const char* status_msg(StatusCode code) {
// 	switch (code) {
// 			case OK: return "OK";
// 			case CREATED: return "Created";
// 			case NO_CONTENT: return "No Content";

// 			case MOVED_PERMANENTLY: return "Moved Permanently";

// 			case BAD_REQUEST: return "Bad Request";
// 			case FORBIDDEN: return "Forbidden";
// 			case NOT_FOUND: return "Not Found";
// 			case METHOD_NOT_ALLOWED: return "Method Not Allowed";
// 			case REQUEST_TIMEOUT: return "Request Timeout";
// 			case CONFLICT: return "Conflict";
// 			case PAYLOAD_TOO_LARGE: return "Payload Too Large";

// 			case INTERNAL_SERVER_ERROR: return "Internal Server Error";
// 			case NOT_IMPLEMENTED: return "Not Implemented";
// 			case BAD_GATEWAY: return "Bad Gateway";
// 			case GATEWAY_TIMEOUT: return "Gateway Timeout\n";
// 			case HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
// 	}
	
// 	return "Internal Server Error";
// }

// inline StatusCode	status_from_error(Error err) {
// 	switch (err) {
// 		case ERR_NONE:
// 			return OK;
// 		case ERR_BAD_REQUEST:
// 		case ERR_MISSING_HOST:
// 		case ERR_DUPLICATE_HEADER:
// 		case ERR_INVALID_CONTENT_LENGTH:
// 		case ERR_CONFLICTING_BODY_HEADERS:
// 			return BAD_REQUEST;
// 		case ERR_UNSUPPORTED_HTTP_VERSION:
// 			return HTTP_VERSION_NOT_SUPPORTED;
// 		case ERR_TE_UNSUPPORTED:
// 			return NOT_IMPLEMENTED;
// 		case ERR_HEADER_TOO_LARGE:
// 		case ERR_BODY_TOO_LARGE:
// 			return PAYLOAD_TOO_LARGE;
// 		case ERR_NOT_FOUND:
// 			return NOT_FOUND;
// 		case ERR_FORBIDDEN:
// 			return FORBIDDEN;
// 		case ERR_METHOD_NOT_ALLOWED:
// 			return METHOD_NOT_ALLOWED;
// 		case ERR_CGI_FAILED:
// 			return BAD_GATEWAY;
// 		case ERR_CGI_TIMEOUT:
// 			return REQUEST_TIMEOUT;
// 		case ERR_INTERNAL:
// 			return INTERNAL_SERVER_ERROR;
// 		default: break;
// 	}
// 	return INTERNAL_SERVER_ERROR;
// }

// }
