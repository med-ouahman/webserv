
#include "http/Parser/headers/headers.hpp"
#include "base/base.hpp"
#include <limits>

namespace http {
namespace parser {

bool isSingleHeader(const std::string& normalized) {
	return normalized == "host"
		|| normalized == "content-length"
		|| normalized == "content-type"
		|| normalized == "transfer-encoding"
		|| normalized == "authorization"
		|| normalized == "proxy-authorization";
}

Error parseContentLength(const std::string& value, usize& out) {
	usize i = 0;
	usize result = 0;
	usize digit;
	usize max = std::numeric_limits<usize>::max();

	if (value.empty())
		return ERR_INVALID_CONTENT_LENGTH;
	while (i < value.size()) {
		if (value[i] < '0' || value[i] > '9')
			return ERR_INVALID_CONTENT_LENGTH;
		digit = static_cast<usize>(value[i] - '0');
		if (result > (max - digit) / 10)
			return ERR_INVALID_CONTENT_LENGTH;
		result = result * 10 + digit;
		++i;
	}
	out = result;
	return ERR_NONE;
}

Error	storeHeader(Request& request, const std::string& name, const std::string& value) {
	std::string normalized = base::toLowerCase(name);
	Header* previous = findHeader(request, normalized);
	Header header;

	if (previous != NULL) {
		if (isSingleHeader(normalized))
			return ERR_DUPLICATE_HEADER;
		previous->value += ", ";
		previous->value += value;
		return ERR_NONE;
	}

	header.key = name;
	header.value = value;
	request.headers.push_back(header);
	return ERR_NONE;
}

SpecialHeader	specialHeader(const std::string& normalized) {
	if (normalized == "host")
		return HEADER_HOST;
	if (normalized == "content-length")
		return HEADER_CONTENT_LENGTH;
	if (normalized == "transfer-encoding")
		return HEADER_TRANSFER_ENCODING;
	if (normalized == "connection")
		return HEADER_CONNECTION;
	return HEADER_NORMAL;
}

Error	handleSpecialHeader(Request& req, const std::string& normalized, const std::string& value) {
	Error err;

	switch (specialHeader(normalized)) {
		case HEADER_NORMAL:
			return ERR_NONE;
		case HEADER_HOST:
			req.host = base::Optional<std::string>(value);
			return ERR_NONE;
		case HEADER_CONTENT_LENGTH:
			usize content_length;
			TRY(parseContentLength(value, content_length), err);
			req.content_length = base::Optional<usize>(content_length);
			return ERR_NONE;
		case HEADER_TRANSFER_ENCODING:
			if (base::toLowerCase(value) != "chunked")
				return ERR_TE_UNSUPPORTED;
			req.chunked = true;
			return ERR_NONE;
		case HEADER_CONNECTION:
			if (base::toLowerCase(value) == "close")
				req.connection = CONNECTION_CLOSE;
			else if (base::toLowerCase(value) == "keep-alive")
				req.connection = CONNECTION_KEEP_ALIVE;
			else
				req.connection = CONNECTION_DEFAULT;
			return ERR_NONE;
	}
	return ERR_NONE;
}

Error	endHeaders(Request& request) {
	if (request.version == HTTP_1_1 && !request.host.has_value())
		return ERR_MISSING_HOST;
	if (request.chunked && request.content_length.has_value())
		return ERR_CONFLICTING_BODY_HEADERS;
	if (request.chunked && request.version != HTTP_1_1)
		return ERR_TE_UNSUPPORTED;
	return ERR_NONE;
}

}
}
