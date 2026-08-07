
#pragma once

#include "http/pipeline/ARequestHandler.hpp"

namespace http {

class Context;

class DeleteHandler : public ARequestHandler {
public:
	explicit DeleteHandler(Context& context);
	virtual ~DeleteHandler();
	virtual Error handle();
};

}
