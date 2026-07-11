#pragma once

#include "http/Error.hpp"
#include "http/pipeline/RequestHandler.hpp"

namespace http {

class Context;

class ErrorHandler : public RequestHandler {
private:
	Error error_;

	static const char* statusMsg(StatusCode code);
	static StatusCode statusFromError(Error error);

public:
	ErrorHandler(Context& context, Error error);
	virtual Error handle();
};

}
