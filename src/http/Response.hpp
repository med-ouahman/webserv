
#pragma once

#include <string>
#include <vector>

#include "base/base.hpp"
#include "base/io/Reader.hpp"
#include "http/Error.hpp"
#include "http/Request.hpp"

namespace http {

enum StatusCode {
	OK = 200,
	CREATED = 201,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	LENGTH_REQUIRED = 411,
	PAYLOAD_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

struct Response {

	std::string body;
	base::io::Reader body_reader;
	std::vector<Header> headers;
	StatusCode status;

	std::string head;

	usize head_offset;
	usize body_offset;
	bool started_;
	bool finished_;

	Response();

	void resetWriteState();
	Error write(char* buffer, usize size, Version version, usize& sent);
	bool started() const;
	bool finished() const;
	bool shouldClose() const;
};

}
