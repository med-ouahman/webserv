
#include "http/Parser/headers/headers.hpp"

#include <cctype>

namespace http {
namespace parser {

static bool	is_space(char c) {
	return c == ' ' || c == '\t';
}

static char	to_lower(char c) {
	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

std::string	lower_name(const std::string& name) {
	std::string out = name;
	usize i = 0;

	while (i < out.size()) {
		out[i] = to_lower(out[i]);
		++i;
	}
	return out;
}

void	trim(std::string& value) {
	usize start = 0;
	usize end = value.size();

	while (start < end && is_space(value[start]))
		++start;
	while (end > start && is_space(value[end - 1]))
		--end;
	value = value.substr(start, end - start);
}

Header*	find_header(Request& request, const std::string& normalized) {
	usize i = 0;

	while (i < request.headers.size()) {
		if (lower_name(request.headers[i].key) == normalized)
			return &request.headers[i];
		++i;
	}
	return NULL;
}

}
}
