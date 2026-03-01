
#pragma once

namespace http {
	enum HTTPStatusCode {
		OK = 200,
		BAD_REQUEST = 401,
		FORBIDDEN = 403,
		NOT_FOUND,
		VERSION_NOT_SUPPORTED = 501
	};
}
