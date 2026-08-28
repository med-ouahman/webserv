
#pragma once

#include "http/pipeline/ARequestHandler.hpp"

namespace http {

class Context;

class UploadHandler : public ARequestHandler {
public:
	explicit UploadHandler(Context& context);
	~UploadHandler();
	Error handle();
};

}
