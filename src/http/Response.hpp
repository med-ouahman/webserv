
#pragma once

#include <map>
#include <string>

#include "base/base.hpp"
#include "http/StatusCode.hpp"
#include "http/Error.hpp"
#include "Headers.hpp"
#include "IBodyProvider.hpp"

namespace http {

struct Response {

	IBodyProvider* body;
	Headers headers;
	StatusCode status;
};

}
