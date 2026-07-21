#pragma once

#include "http/pipeline/RequestHandler.hpp"

namespace http {

struct MimeEntry {
	const char* extension;
	const char* type;
};

class Context;

class StaticFileHandler : public ARequestHandler {
public:
	explicit StaticFileHandler(Context& context);
	virtual ~StaticFileHandler();
	virtual Error handle();
};

}
