
#pragma once

#include <map>
#include "StatusCode.hpp"
#include "foundation/baselib/Base.hpp"
#include "common/Headers.hpp"
#include "Base.hpp"

namespace http {

struct Request {
	Headers headers_;
	std::map<std::string, std::string> headers;
	std::string body;

	Request();
	static Request error(StatusCode code);
	isize produce(Base::io::Writer* writer);
};

}
