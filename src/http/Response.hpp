
#pragma once

#include <map>
#include <string>

#include "base/base.hpp"
#include "http/StatusCode.hpp"
#include "http/Error.hpp"
#include "Headers.hpp"
#include "IBodyProvider.hpp"
#include "BodyEncoder.hpp"

namespace http {


struct Response {

	StatusCode status;
	Headers headers;
	IBodyProvider* body;
	body::BodyEncoder encoder;

	Response(): body(NULL) {}
	~Response() {
		
		if (body) delete body;
		body = NULL;
	}
};

}
