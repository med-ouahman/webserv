
#pragma once

#include <map>
#include <string>

#include "base/base.hpp"
#include "http/StatusCode.hpp"
#include "http/Error.hpp"

namespace http {

struct Response {

	// BodyEncoder encoder;
	std::map<std::string, std::string> headers;
	StatusCode status;
};

}
