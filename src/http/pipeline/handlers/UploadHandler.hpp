
#pragma once

#include "http/pipeline/RequestHandler.hpp"

namespace http {

class Context;

class UploadHandler : public RequestHandler {
public:
	explicit UploadHandler(Context& context);
	virtual ~UploadHandler();
	virtual Error handle();
};

}
