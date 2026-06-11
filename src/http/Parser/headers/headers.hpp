#pragma once

#include <string>

#include "http/Error.hpp"
#include "http/Request.hpp"

namespace http {
namespace parser {

enum SpecialHeader {
	HEADER_NORMAL,
	HEADER_HOST,
	HEADER_CONTENT_LENGTH,
	HEADER_TRANSFER_ENCODING,
	HEADER_CONNECTION
};

std::string	lower_name(const std::string& name);
void		trim(std::string& value);

Header*		find_header(Request& request, const std::string& normalized);
Error		store_header(Request& request, const std::string& name, const std::string& value);
bool		is_single_header(const std::string& normalized);
bool		parse_content_length(const std::string& value, usize& out);
SpecialHeader	special_header(const std::string& normalized);
Error		handle_special_header(Request& request, const std::string& normalized, const std::string& value);
Error		end_headers(Request& request);

}
}
