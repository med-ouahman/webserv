#pragma once

#include "http/pipeline/RequestHandler.hpp"

namespace http {

class Context;

class RedirectHandler : public ARequestHandler {
public:
	explicit RedirectHandler(Context& context);
	virtual ~RedirectHandler();
	virtual Error handle();
};

}
