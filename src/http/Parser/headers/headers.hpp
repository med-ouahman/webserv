#pragma once

#include <string>

#include "http/Error.hpp"
#include "http/Request/Request.hpp"

namespace http {
namespace parser {

enum SpecialHeader {
	HEADER_NORMAL,
	HEADER_HOST,
	HEADER_CONTENT_LENGTH,
	HEADER_TRANSFER_ENCODING,
	HEADER_CONNECTION
};

void		trim(std::string& value);

Header*		findHeader(Request& request, const std::string& normalized);
Error		storeHeader(Request& request, const std::string& name, const std::string& value);
bool		isSingleHeader(const std::string& normalized);
Error		parseContentLength(const std::string& value, usize& out);
SpecialHeader	specialHeader(const std::string& normalized);
Error		handleSpecialHeader(Request& request, const std::string& normalized, const std::string& value);
Error		endHeaders(Request& request);

}
}
