
#include "http/Parser/headers/headers.hpp"
#include <limits>

namespace http {
namespace parser {

bool	is_single_header(const std::string& normalized) {
	return normalized == "host"
		|| normalized == "content-length"
		|| normalized == "content-type"
		|| normalized == "transfer-encoding"
		|| normalized == "authorization"
		|| normalized == "proxy-authorization";
}

bool	parse_content_length(const std::string& value, usize& out) {
	usize i = 0;
	usize result = 0;
	usize digit;
	usize max = std::numeric_limits<usize>::max();

	if (value.empty())
		return false;
	while (i < value.size()) {
		if (value[i] < '0' || value[i] > '9')
			return false;
		digit = static_cast<usize>(value[i] - '0');
		if (result > (max - digit) / 10)
			return false;
		result = result * 10 + digit;
		++i;
	}
	out = result;
	return true;
}

Error	store_header(Request& request, const std::string& name, const std::string& value) {
	std::string normalized = lower_name(name);
	Header* previous = find_header(request, normalized);
	Header header;

	if (previous != NULL) {
		if (is_single_header(normalized))
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

SpecialHeader	special_header(const std::string& normalized) {
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

Error	handle_special_header(Request& request, const std::string& normalized, const std::string& value) {
	usize content_length;

	switch (special_header(normalized)) {
		case HEADER_NORMAL:
			return ERR_NONE;
		case HEADER_HOST:
			request.host = base::Optional<std::string>(value);
			return ERR_NONE;
		case HEADER_CONTENT_LENGTH:
			if (!parse_content_length(value, content_length))
				return ERR_INVALID_CONTENT_LENGTH;
			request.content_length = base::Optional<usize>(content_length);
			return ERR_NONE;
		case HEADER_TRANSFER_ENCODING:
			if (lower_name(value) != "chunked")
				return ERR_TE_UNSUPPORTED;
			request.chunked = true;
			return ERR_NONE;
		case HEADER_CONNECTION:
			if (lower_name(value) == "close")
				request.connection = CONNECTION_CLOSE;
			else if (lower_name(value) == "keep-alive")
				request.connection = CONNECTION_KEEP_ALIVE;
			else
				request.connection = CONNECTION_DEFAULT;
			return ERR_NONE;
	}
	return ERR_NONE;
}

Error	end_headers(Request& request) {
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
