#pragma once

#include "http/Error.hpp"
#include "http/pipeline/ARequestHandler.hpp"

namespace http {

class Context;

class ErrorHandler : public ARequestHandler {
private:
	Error error_;

	void setAllowedMethods();
	static StatusCode statusFromError(Error error);

public:
	ErrorHandler(Context& context, Error error);
	Error handle();
};

}
