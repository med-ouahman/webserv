#pragma once

#include "http/pipeline/ARequestHandler.hpp"

namespace http {

class Context;

class DirectoryHandler : public ARequestHandler {
public:
	explicit DirectoryHandler(Context& context);
	virtual ~DirectoryHandler();
	virtual Error handle();
};

}
