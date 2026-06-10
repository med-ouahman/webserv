
#pragma once

#include <map>
#include <string>

#include "base/base.hpp"
#include "http/StatusCode.hpp"
#include "http/Error.hpp"
#include "BodyEncoder.hpp"

namespace http {

struct Response {
	IBodyProvider* body_p;
	body::BodyEncoder encoder;
	std::string body;
	std::map<std::string, std::string> headers;
	StatusCode status;
};

}
