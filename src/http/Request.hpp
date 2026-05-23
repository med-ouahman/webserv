
#pragma once

#include <map>
#include "StatusCode.hpp"
#include "base/base.hpp"
#include "Base.hpp"

namespace http {

struct Response {
	std::map<std::string, std::string> headers;
	std::string body;

	Response();
	static Response error(StatusCode code);
	isize produce(Base::io::Writer* writer);
};

}
