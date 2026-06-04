#include "http/parser/body/body.hpp"
#include "http/Context.hpp"

namespace http {

Error Context::finish_body() {
	if (body_writer.file_created()) {
		if (!body_writer.flush())
			return ERR_INTERNAL;
		request.body.reset(body_writer.path());
	} else {
		request.body.reset(body_writer.data(), body_writer.size());
	}
	body_received = 0;
	state_ = PROCESSING;
	return ERR_NONE;
}

}
