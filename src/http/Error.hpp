#pragma once

#define TRY(expr, fail) \
	do { \
		err = (expr); \
		if (err != http::ERR_NONE) \
			return fail; \
	} while (0)

namespace http {

class Context;

enum Error {

	ERR_NONE = 0,

	/* parser/request */
	ERR_BAD_REQUEST,
	ERR_UNSUPPORTED_HTTP_VERSION,
	ERR_MISSING_HOST,

	/* Parsing */
	ERR_DUPLICATE_HEADER,
	ERR_INVALID_CONTENT_LENGTH,
	ERR_TE_UNSUPPORTED,
	ERR_CONFLICTING_BODY_HEADERS,

	/* Size bound checking */
	ERR_HEADER_TOO_LARGE,
	ERR_BODY_TOO_LARGE,
	ERR_REQUEST_TIMEOUT,

	/* handlers */
	ERR_NOT_FOUND,
	ERR_FORBIDDEN,
	ERR_METHOD_NOT_ALLOWED,
	ERR_LENGTH_REQUIRED,

	/* cgi */
	ERR_CGI_FAILED,
	ERR_CGI_TIMEOUT,
	ERR_BAD_GATEWAY,

	ERR_INTERNAL,
};

}
