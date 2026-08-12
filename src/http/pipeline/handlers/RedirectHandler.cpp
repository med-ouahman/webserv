
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
	: ARequestHandler(context) {}

RedirectHandler::~RedirectHandler() {}

Error RedirectHandler::handle() {
	int code = decision().location->redirect.return_code;
	std::string target = decision().location->redirect.return_target;

	if (decision().path_type == directory) {
		code = 301;
		target = request().path + "/";
		if (request().query.has_value())
			target += "?" + request().query.value;
	}
	setStatus(redirectStatus(code));
	setBodyFixed("");
	setHeader("Location", target);
	setContentLength();
	setConnection();
	setDate();
	responseReady();
	return ERR_NONE;
}

}
