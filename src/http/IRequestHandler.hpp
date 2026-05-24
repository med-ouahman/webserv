#pragma once

namespace http {
class IRequestHandler {

public:
	void handle() = 0;
	bool done() = 0;
};
}
