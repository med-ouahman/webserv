
#pragma once

namespace http {
	enum HTTPStatusCode {
		NONE = 0,
		OK = 200,
		BAD_REQUEST = 400,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		METHOD_NOT_ALLOWED,
		INTERNAL_SERVER_ERROR = 500,
		VERSION_NOT_SUPPORTED = 501
	};
}
