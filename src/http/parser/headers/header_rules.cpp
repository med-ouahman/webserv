
#include "http/parser/headers/headers.hpp"

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

	if (value.empty())
		return false;
	while (i < value.size()) {
		if (value[i] < '0' || value[i] > '9')
			return false;
		result = result * 10 + static_cast<usize>(value[i] - '0');
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
			return DUPLICATE_HEADER;
		previous->value += ", ";
		previous->value += value;
		return NONE;
	}

	header.key = name;
	header.value = value;
	request.headers.push_back(header);
	return NONE;
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
			return NONE;
		case HEADER_HOST:
			request.host = base::Optional<std::string>(value);
			return NONE;
		case HEADER_CONTENT_LENGTH:
			if (!parse_content_length(value, content_length))
				return INVALID_CONTENT_LENGTH;
			request.content_length = base::Optional<usize>(content_length);
			return NONE;
		case HEADER_TRANSFER_ENCODING:
			if (lower_name(value) == "chunked")
				request.transfer_encoding = TE_CHUNKED;
			else
				request.transfer_encoding = TE_UNSUPPORTED;
			return NONE;
		case HEADER_CONNECTION:
			if (lower_name(value) == "close")
				request.connection = CONNECTION_Close;
			else if (lower_name(value) == "keep-alive")
				request.connection = CONNECTION_KEEP_ALIVE;
			else
				request.connection = CONNECTION_DEFAULT;
			return NONE;
	}
	return NONE;
}

Error	end_headers(Request& request, ContextState& state) {
	if (request.version == HTTP_1_1 && !request.host.has_value())
		return MISSING_HOST;
	if (request.transfer_encoding != TE_NONE
		&& request.content_length.has_value())
		return CONFLICTING_BODY_HEADERS;
	if (request.content_length.has_value()
		&& request.content_length.value > 0)
		state = BODY;
	else
		state = PROCESSING;
	return NONE;
}

}
}
