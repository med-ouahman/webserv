
#include "base/base.hpp"
#include "http/Context.hpp"

#include "http/Parser/Parser.hpp"
#include "http/Parser/body/temp_storage.hpp"
#include "http/pipeline/pipeline.hpp"
#include "http/pipeline/handlers/ErrorHandler.hpp"
#include "http/routing/Routing.hpp"
#include "http/Error.hpp"

#include "CgiHandler.hpp"
#include "Logger.hpp"

#include <cstdio>
#include <sstream>

namespace http {

Context::Context(const std::vector<const config::ServerConfig*>& servers,
		usize conn_id, usize request_id, RuntimeServices& services)
	: actor(),
	  info(servers, conn_id, request_id),
	  error(ERR_NONE),
	  active_requests(0),
	  services_(services) {
	advanceCycle();
}

Context::~Context() { actor.reset(); }

void Context::advanceCycle() {
	actor.reset();
	info.dispatch = base::Optional<DispatchInfo>();
	error = ERR_NONE;
	state_ = PARSING;
	action_ = AC_READ;
	if (active_requests > limits::MAX_REQUESTS_PER_CONN)
		setError(ERR_TOO_MANY_REQUESTS);
	++active_requests;
}

usize Context::consume(BufferView& buff) {
	Error err;
	usize processed = 0;

	if (buff.empty() or action_ != AC_READ) return processed;

	if (state_ == PARSING or state_ == PROCESSING) {
		ContextState prev = state_;
		TRY(actor.parser.progress(*this, buff, processed),
				(setError(err), processed));

		if (prev == PARSING and state_ == PROCESSING) {
			services_.logger.log(logger::Debug,
				"http: parsing complete -> processing", false);
			TRY(resolveDispatch(), (setError(err), processed));
			TRY(actor.parser.prepareBodyStorage(*this),
					(setError(err), processed));
			TRY(createHandler(), (setError(err), processed));
			if (!buff.empty())
				TRY(actor.parser.progress(*this, buff, processed),
						(setError(err), processed));
		}
	}
	if (state_ == PROCESSING)
		action_ = AC_NONE;

	return processed;
}

void Context::process() {
	Error err;

	if (state_ == PROCESSING)
		TRY(actor.handler->handle(), (setError(err), void()));
}

ContextAction Context::nextAction() const { return action_; }

void Context::timeout() {
	
	if (action_ == AC_READ and actor.parser.timedOut()) {
		setError(ERR_REQUEST_TIMEOUT);
		return ;
	}
	
	if (state_ == PROCESSING and actor.handler != NULL)
		actor.handler->monitor();
}

usize Context::produce(char *buffer, usize size) {
	Error err;
	usize sent = 0;

	if (state_ != DONE or action_ != AC_WRITE) return 0;
	TRY(actor.response.write(buffer, size, actor.request.version, sent),
		(handleResponseFailure(err)));
	if (actor.response.finished() and actor.response.shouldClose())
		action_ = AC_CLOSE;
	return sent;
}

}
