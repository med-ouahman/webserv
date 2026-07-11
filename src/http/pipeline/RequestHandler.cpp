
#include "http/pipeline/RequestHandler.hpp"
#include "http/Context.hpp"

#include <sstream>
#include <ctime>

namespace http {

RequestHandler::RequestHandler(Context& context)
	: context_(context) {}

RequestHandler::~RequestHandler() {}

void RequestHandler::setStatus(StatusCode status) {
	context_.response.status = status;
}

void RequestHandler::setHeader(const std::string& key,
		const std::string& value) {
	context_.response.headers[key] = value;
}

void RequestHandler::setBody(const std::string& body) {
	context_.response.body = body;
	context_.response.body_reader.reset();
}

Error RequestHandler::setBodyFile(const std::string& path) {
	context_.response.body.clear();
	if (!context_.response.body_reader.reset(path))
		return ERR_NOT_FOUND;
	return ERR_NONE;
}

void RequestHandler::setContentType(const std::string& type) {
	setHeader("Content-Type", type);
}

void RequestHandler::setContentLength() {
	setContentLength(context_.response.body.size());
}

void RequestHandler::setContentLength(usize size) {
	std::ostringstream length;

	length << size;
	setHeader("Content-Length", length.str());
}

void RequestHandler::setConnection() {
	if (context_.request.version == HTTP_1_1
		&& context_.request.connection != CONNECTION_CLOSE) {
		setHeader("Connection", "keep-alive");
		return ;
	}
	if (context_.request.version == HTTP_1_0
		&& context_.request.connection == CONNECTION_KEEP_ALIVE) {
		setHeader("Connection", "keep-alive");
		return ;
	}
	setHeader("Connection", "close");
}

void RequestHandler::setDate() {
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

const Decision& RequestHandler::decision() const {
	return context_.route.value;
}

Request& RequestHandler::request() {
	return context_.request;
}

Response& RequestHandler::response() {
	return context_.response;
}

}
