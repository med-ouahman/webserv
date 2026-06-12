#pragma once

#include "ServerContext.hpp"
#include "IRequestHandler.hpp"

namespace http {

struct Request;
struct ResolutionResult;

class HandlerFactory {
private:
	ServerContext server_ctx;

public:
	enum RequestType {
		StaticFile,
		Directory,
		Delete,
		Cgi
	};

	HandlerFactory(ServerContext& ctx);
	~HandlerFactory();
	
	IRequestHandler* create(ResolutionResult& result, Request& req, RequestType type);
};

}
