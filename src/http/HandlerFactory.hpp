#pragma once

#include "ServerContext.hpp"
#include "IRequestHandler.hpp"

namespace http {

struct Request;
struct ResolutionResult;

class HandlerFactory {
private:
	ServerContext server_ctx;
	Context& protocol_;

	HandlerFactory(const HandlerFactory&);
	HandlerFactory& operator=(const HandlerFactory&);
	
public:
	enum RequestType {
		StaticFile,
		Directory,
		Delete,
		Cgi
	};

	HandlerFactory(ServerContext& ctx, Context& proto);
	~HandlerFactory();
	
	IRequestHandler* create(ResolutionResult& result, Request& req, RequestType type);
};

}
