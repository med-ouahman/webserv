
#pragma once

#include "http/Error.hpp"
#include "http/Response.hpp"

#include <string>

namespace http {

class Context;
struct DispatchInfo;
struct Request;

class ARequestHandler {

protected:
	Context& context_;

	void setStatus(StatusCode status);
	void setHeader(const std::string& key, const std::string& value);
	void eraseHeader(const std::string& key);

	void setBodyFixed(const std::string& body);
	Error setBodyFile(const std::string& path);

	void setContentType(const std::string& type);
	void setContentTypeFromPath(const std::string& path);
	void setContentLength();
	void setContentLength(usize size);
	void setConnection();
	void setDate();
	void responseReady();
	const DispatchInfo& decision() const;
	Request& request();
	Response& response();

public:
	explicit ARequestHandler(Context& context);
	virtual Error handle() = 0;
	virtual Error timeout();
	virtual ~ARequestHandler();
};

}
