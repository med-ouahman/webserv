
#pragma once

namespace http {

struct Response {
	StatusCode status;
	std::map<std::string, std::string> headers;
	std::string body;

	Response();
	static Response error(StatusCode code);
	isize produce(Base::io::Writer* writer);
};

}
