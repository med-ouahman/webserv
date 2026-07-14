
#include "http/pipeline/handlers/RedirectHandler.hpp"
#include "http/Context.hpp"

namespace http {

namespace {

static StatusCode redirectStatus(int code) {
	if (code == 301)
		return MOVED_PERMANENTLY;
	if (code == 302)
		return FOUND;
	if (code == 303)
		return SEE_OTHER;
	if (code == 307)
		return TEMPORARY_REDIRECT;
	if (code == 308)
		return PERMANENT_REDIRECT;
	return MOVED_PERMANENTLY;
}

}

RedirectHandler::RedirectHandler(Context& context)
	: RequestHandler(context) {}

RedirectHandler::~RedirectHandler() {}

Error RedirectHandler::handle() {
	setStatus(redirectStatus(decision().location->redirect.return_code));
	setBodyFixed("");
	setHeader("Location", decision().location->redirect.return_target);
	setContentLength();
	setConnection();
	setDate();
	return ERR_NONE;
}

}
