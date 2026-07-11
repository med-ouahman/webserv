#include "http/Parser/parser.hpp"
#include "http/Context.hpp"

namespace http {

Error Parser::finishBody(Context& ctx) {
	if (bodyWriter.file_created()) {
		if (!bodyWriter.flush())
			return ERR_INTERNAL;
		ctx.request.body.reset(bodyWriter.path());
	} else {
		ctx.request.body.reset(bodyWriter.data(), bodyWriter.size());
	}
	body_received = 0;
	ctx.state_ = PROCESSING;
	ctx.action_ = AC_WRITE;
	return ERR_NONE;
}

}
