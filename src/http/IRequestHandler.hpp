#pragma once

namespace http {

class IRequestHandler {

public:
	virtual void handle() = 0;
	virtual bool finished() const = 0;
	virtual ~IRequestHandler() {};
};

}
