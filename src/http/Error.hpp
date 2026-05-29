
#pragma once

namespace http {

enum Error {

	NONE = 0,

	/* parser/request */
	BAD_REQUEST,
	UNSUPPORTED_HTTP_VERSION,
	MISSING_HOST,

	/* Parsing */
	DUPLICATE_HEADER,
	INVALID_CONTENT_LENGTH,
	CONFLICTING_BODY_HEADERS,

	/* Size bound checking */
	HEADER_TOO_LARGE,
	BODY_TOO_LARGE,

	/* handlers */
	NOT_FOUND,
	FORBIDDEN,
	METHOD_NOT_ALLOWED,

	/* cgi */
	CGI_FAILED,
	CGI_TIMEOUT,

	INTERNAL,
};

}
