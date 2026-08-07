
#include "http/Parser/headers/headers.hpp"

#include <cctype>

namespace http {
namespace parser {

static bool	isSpace(char c) { return c == ' ' || c == '\t'; }

static char	toLower(char c) {
	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

std::string	lowerName(const std::string& name) {
	std::string out = name;
	usize i = 0;

	while (i < out.size()) {
		out[i] = toLower(out[i]);
		++i;
	}
	return out;
}

void	trim(std::string& value) {
	usize start = 0;
	usize end = value.size();

	while (start < end && isSpace(value[start]))
		++start;
	while (end > start && isSpace(value[end - 1]))
		--end;
	value = value.substr(start, end - start);
}

Header*	findHeader(Request& request, const std::string& normalized) {
	usize i = 0;

	while (i < request.headers.size()) {
		if (lowerName(request.headers[i].key) == normalized)
			return &request.headers[i];
		++i;
	}
	return NULL;
}

}
}
