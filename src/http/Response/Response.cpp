
#include "http/Response/Response.hpp"

#include <algorithm>
#include <cstring>
#include <sstream>

namespace http {

namespace {

static const char* statusMsg(StatusCode code) {
	switch (code) {
		case OK: return "OK";
		case CREATED: return "Created";
		case NO_CONTENT: return "No Content";
		case MOVED_PERMANENTLY: return "Moved Permanently";
		case FOUND: return "Found";
		case SEE_OTHER: return "See Other";
		case TEMPORARY_REDIRECT: return "Temporary Redirect";
		case PERMANENT_REDIRECT: return "Permanent Redirect";
		case BAD_REQUEST: return "Bad Request";
		case FORBIDDEN: return "Forbidden";
		case NOT_FOUND: return "Not Found";
		case METHOD_NOT_ALLOWED: return "Method Not Allowed";
		case REQUEST_TIMEOUT: return "Request Timeout";
		case CONFLICT: return "Conflict";
		case LENGTH_REQUIRED: return "Length Required";
		case PAYLOAD_TOO_LARGE: return "Payload Too Large";
		case TOO_MANY_REQUESTS: return "Too Many Requests";
		case INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case NOT_IMPLEMENTED: return "Not Implemented";
		case BAD_GATEWAY: return "Bad Gateway";
		case GATEWAY_TIMEOUT: return "Gateway timeout";
		case HTTP_VERSION_NOT_SUPPORTED:
			return "HTTP Version Not Supported";
	}
	return "Internal Server Error";
}

static bool mayHaveContentLength(StatusCode status) {
	return status != NO_CONTENT;
}

static std::string serializeResponseHead(const Response& response,
		Version ver) {
	std::ostringstream out;
	std::vector<Header>::const_iterator it;
	bool has_content_length = false;

	if (ver == HTTP_1_0) {
		out << "HTTP/1.0 " << static_cast<int>(response.status)
			<< " " << statusMsg(response.status) << "\r\n";
	}
	else {
		out << "HTTP/1.1 " << static_cast<int>(response.status)
			<< " " << statusMsg(response.status) << "\r\n";
	}
	for (it = response.headers.begin(); it != response.headers.end(); ++it) {
		if (it->key == "Content-Length")
			has_content_length = true;
		out << it->key << ": " << it->value << "\r\n";
	}
	if (!has_content_length
		&& mayHaveContentLength(response.status)
		&& response.body_reader.type() == base::io::Reader::NONE)
		out << "Content-Length: " << response.body.size() << "\r\n";
	out << "\r\n";
	return out.str();
}

static const Header* findHeader(const Response& response,
		const std::string& key) {
	usize i = 0;

	while (i < response.headers.size()) {
		if (response.headers[i].key == key)
			return &response.headers[i];
		++i;
	}
	return NULL;
}

static bool responseShouldClose(const Response& response) {
	const Header* header = findHeader(response, "Connection");

	if (header == NULL)
		return true;
	return header->value != "keep-alive";
}

static Error writeFixedReader(Response& response, char* buffer, usize size,
		usize& sent) {
	base::Expected<usize, base::io::Error> chunk =
		response.body_reader.read(buffer + sent, size - sent);

	if (!chunk)
		return ERR_INTERNAL;
	if (chunk.value() == 0) {
		response.finished_ = true;
		return ERR_NONE;
	}
	sent += chunk.value();
	return ERR_NONE;
}

static Error writeFixedString(Response& response, char* buffer, usize size,
		usize& sent) {
	usize remaining = response.body.size() - response.body_offset;
	usize amount;

	if (remaining == 0)
		return ERR_NONE;
	amount = std::min(remaining, size - sent);
	if (amount == 0)
		return ERR_NONE;
	::memcpy(buffer + sent, response.body.data() + response.body_offset,
		amount);
	response.body_offset += amount;
	sent += amount;
	if (response.body_offset == response.body.size())
		response.finished_ = true;
	return ERR_NONE;
}

}

Response::Response()
	: body(),
	  body_reader(),
	  headers(),
	  status(OK),
	  head(),
	  head_offset(0),
	  body_offset(0),
	  started_(false),
	  finished_(false) {}

void Response::reset() {
	body.clear();
	body_reader.reset();
	headers.clear();
	status = OK;
	resetWriteState();
}

void Response::resetWriteState() {
	head.clear();
	head_offset = 0;
	body_offset = 0;
	started_ = false;
	finished_ = false;
}

static Error writeResponseHead(Response& response, char* buffer, usize size,
		Version version, usize& sent) {
	usize remaining;
	usize amount;

	if (response.head.empty())
		response.head = serializeResponseHead(response, version);
	remaining = response.head.size() - response.head_offset;
	if (remaining == 0)
		return ERR_NONE;
	amount = std::min(remaining, size - sent);
	if (amount == 0)
		return ERR_NONE;
	::memcpy(buffer + sent, response.head.data() + response.head_offset,
		amount);
	response.head_offset += amount;
	sent += amount;
	return ERR_NONE;
}

static Error writeResponseBody(Response& response, char* buffer, usize size,
		usize& sent) {
	if (sent == size)
		return ERR_NONE;
	if (response.body_reader.type() != base::io::Reader::NONE)
		return writeFixedReader(response, buffer, size, sent);
	return writeFixedString(response, buffer, size, sent);
}

Error Response::write(char* buffer, usize size, Version version, usize& sent) {
	Error err;

	TRY(writeResponseHead(*this, buffer, size, version, sent), err);
	if (head_offset != head.size())
		return ERR_NONE;
	TRY(writeResponseBody(*this, buffer, size, sent), err);
	if (head_offset == head.size()
		&& body_reader.type() == base::io::Reader::NONE
		&& body_offset == body.size())
		finished_ = true;
	if (sent > 0)
		started_ = true;
	return ERR_NONE;
}

bool Response::started() const {
	return started_;
}

bool Response::finished() const {
	return finished_;
}

bool Response::shouldClose() const {
	return responseShouldClose(*this);
}

}
