
#pragma once

namespace http {

enum Error {

	NONE = 0,

	/* parser/request */
	BAD_REQUEST,
	UNSUPPORTED_HTTP_VERSION,
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
