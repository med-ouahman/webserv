
#pragma once

namespace http {

enum Error {

	NONE = 0,

<<<<<<< HEAD
	ERR_BAD_REQUEST,
	ERR_UNSUPPORTED_HTTP_VERSION,
	ERR_MISSING_HOST,
=======
	/* parser/request */
	BAD_REQUEST,
	UNSUPPORTED_HTTP_VERSION,
	MISSING_HOST,
>>>>>>> shady

	/* Parsing */
	DUPLICATE_HEADER,
	INVALID_CONTENT_LENGTH,
	CONFLICTING_BODY_HEADERS,

	/* Size bound checking */
	HEADER_TOO_LARGE,
	BODY_TOO_LARGE,

<<<<<<< HEAD
	// /* handlers */
	// NOT_FOUND,
	// FORBIDDEN,
	// METHOD_NOT_ALLOWED,

	// /* cgi */
	// CGI_FAILED,
	// CGI_TIMEOUT,
=======
	/* handlers */
	NOT_FOUND,
	FORBIDDEN,
	METHOD_NOT_ALLOWED,

	/* cgi */
	CGI_FAILED,
	CGI_TIMEOUT,
>>>>>>> shady

	INTERNAL,
};

}
