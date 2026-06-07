
#pragma once

#include <map>
#include <string>

#include "base/base.hpp"
#include "http/StatusCode.hpp"
#include "http/Error.hpp"

namespace http {

struct Response {

<<<<<<< HEAD
	// BodyEncoder encoder;
=======
	std::string body;
>>>>>>> shady
	std::map<std::string, std::string> headers;
	StatusCode status;
};

}
