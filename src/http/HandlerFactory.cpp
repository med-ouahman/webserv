
#include "HandlerFactory.hpp"
#include "CGIRequestHandler.hpp"

namespace http {

HandlerFactory::HandlerFactory(ServerContext& ctx)
	: server_ctx(ctx) {}

HandlerFactory::~HandlerFactory() {}

IRequestHandler* HandlerFactory::create(ResolutionResult& result, Request& req, RequestType type) {
	switch (type) {
		case Cgi:
			return new CGIRequestHandler(result, req, server_ctx);
		default:
			return NULL;
	}
	return NULL;
}

}
