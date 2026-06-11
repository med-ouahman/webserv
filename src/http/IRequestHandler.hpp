#pragma once

namespace http {

class IRequestHandler {

public:
	virtual void handle() = 0;
	virtual bool finished() = 0;
	~IRequestHandler() {};
};

}
