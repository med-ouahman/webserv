#include "http/Parser/body/body.hpp"
#include "http/Context.hpp"

namespace http {

Error ParserState::finish_body(Context& ctx) {
	if (body_writer.file_created()) {
		if (!body_writer.flush())
			return ERR_INTERNAL;
		ctx.request.body.reset(body_writer.path());
	} else {
		ctx.request.body.reset(body_writer.data(), body_writer.size());
	}
	body_received = 0;
	ctx.state_ = PROCESSING;
	ctx.action_ = AC_WORK;
	return ERR_NONE;
}

}
