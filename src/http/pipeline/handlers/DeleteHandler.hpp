
#pragma once

#include "http/pipeline/RequestHandler.hpp"

namespace http {

class Context;

class DeleteHandler : public ARequestHandler {
public:
	explicit DeleteHandler(Context& context);
	virtual ~DeleteHandler();
	virtual Error handle();
};

}
