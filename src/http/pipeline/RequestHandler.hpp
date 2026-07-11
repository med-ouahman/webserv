
#pragma once

#include "http/Error.hpp"
#include "http/Response.hpp"

#include <string>

namespace http {

class Context;
struct Decision;
struct Request;

class RequestHandler {

protected:
	Context& context_;

	void setStatus(StatusCode status);
	void setHeader(const std::string& key, const std::string& value);
	void setBody(const std::string& body);
	Error setBodyFile(const std::string& path);
	void setContentType(const std::string& type);
	void setContentLength();
	void setContentLength(usize size);
	void setConnection();
	void setDate();
	const Decision& decision() const;
	Request& request();
	Response& response();

public:
	explicit RequestHandler(Context& context);
	virtual Error handle() = 0;
	virtual ~RequestHandler();
};

}
