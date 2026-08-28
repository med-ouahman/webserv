#include "http/Parser/Parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::finishBody(Context& ctx) {
	Request& req = ctx.actor.request;

	if (bodyWriter.file_created()) {
		if (!bodyWriter.flush())
			return ERR_INTERNAL;
		req.body.reset(bodyWriter.path());
	}
	else req.body.reset(bodyWriter.data(), bodyWriter.size());

	body_received = 0;
	return ERR_NONE;
}

}
