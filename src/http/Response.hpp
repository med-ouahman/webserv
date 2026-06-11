
#pragma once

#include <map>
#include <string>

#include "base/base.hpp"
#include "http/StatusCode.hpp"
#include "http/Error.hpp"
#include "BodyEncoder.hpp"
#include "common/Headers.hpp"

namespace http {

struct Response {
	IBodyProvider* body;
	body::BodyEncoder encoder;
	Headers		headers;
	StatusCode status;
};

}
