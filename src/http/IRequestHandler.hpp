#pragma once

namespace http {
class IRequestHandler {

public:
	virtual void handle() = 0;
	virtual bool done() = 0;
	virtual ~IRequestHandler() {}
};

}
