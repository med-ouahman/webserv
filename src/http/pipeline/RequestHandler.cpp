
#include "http/pipeline/RequestHandler.hpp"
#include "http/Context.hpp"

#include <sstream>
#include <ctime>

namespace http {

ARequestHandler::ARequestHandler(Context& context)
	: context_(context) {}

ARequestHandler::~ARequestHandler() {}

Error ARequestHandler::timeout() {
	return ERR_NONE;
}

void ARequestHandler::setStatus(StatusCode status) {
	context_.actor.response.status = status;
}

void ARequestHandler::setHeader(const std::string& key,
		const std::string& value) {
	usize i = 0;
	Header header;

	while (i < context_.actor.response.headers.size()) {
		if (context_.actor.response.headers[i].key == key) {
			context_.actor.response.headers[i].value = value;
			return ;
		}
		++i;
	}
	header.key = key;
	header.value = value;
	context_.actor.response.headers.push_back(header);
}

void ARequestHandler::eraseHeader(const std::string& key) {
	std::vector<Header>::iterator it = context_.actor.response.headers.begin();

	while (it != context_.actor.response.headers.end()) {
		if (it->key == key)
			it = context_.actor.response.headers.erase(it);
		else
			++it;
	}
}

void ARequestHandler::setBodyFixed(const std::string& body) {
	context_.actor.response.body = body;
	context_.actor.response.body_reader.reset();
}

Error ARequestHandler::setBodyFile(const std::string& path) {
	context_.actor.response.body.clear();
	if (!context_.actor.response.body_reader.reset(path))
		return ERR_NOT_FOUND;
	return ERR_NONE;
}

void ARequestHandler::setContentType(const std::string& type) {
	setHeader("Content-Type", type);
}

void ARequestHandler::setContentLength() {
	setContentLength(context_.actor.response.body.size());
}

void ARequestHandler::setContentLength(usize size) {
	std::ostringstream length;

	length << size;
	setHeader("Content-Length", length.str());
}

void ARequestHandler::setConnection() {
	if (context_.actor.request.version == HTTP_1_1
		&& context_.actor.request.connection != CONNECTION_CLOSE) {
		setHeader("Connection", "keep-alive");
		return ;
	}
	if (context_.actor.request.version == HTTP_1_0
		&& context_.actor.request.connection == CONNECTION_KEEP_ALIVE) {
		setHeader("Connection", "keep-alive");
		return ;
	}
	setHeader("Connection", "close");
}

void ARequestHandler::setDate() {
	char buffer[64];
	time_t now = time(NULL);
	struct tm* time_info = gmtime(&now);

	if (time_info == NULL)
		return ;
	if (strftime(buffer, sizeof(buffer),
		"%a, %d %b %Y %H:%M:%S GMT", time_info) == 0)
		return ;
	setHeader("Date", buffer);
}

void ARequestHandler::responseReady() {
	context_.responseReady();
}

const DispatchInfo& ARequestHandler::decision() const {
	return context_.info.dispatch.value;
}

Request& ARequestHandler::request() { return context_.actor.request; }

Response& ARequestHandler::response() { return context_.actor.response; }

}
