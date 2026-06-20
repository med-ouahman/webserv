
#include "HandlerFactory.hpp"
#include "CgiHandler.hpp"

namespace http {

HandlerFactory::HandlerFactory(ServerContext& ctx, Context& proto)
	: server_ctx(ctx),
	protocol_(proto) {}

HandlerFactory::~HandlerFactory() {}

IRequestHandler* HandlerFactory::create(ResolutionResult& result, Request& req, RequestType type) {
	
	switch (type) {
		case Cgi:
			return new CgiHandler(result, req, *server_ctx.poller, protocol_);
		default:
			return NULL;
	}

	return NULL;
}

}
