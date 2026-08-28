
#include "http/Parser/headers/headers.hpp"


namespace http {
namespace parser {

static bool	isSpace(char c) { return c == ' ' || c == '\t'; }

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
		if (base::toLowerCase(request.headers[i].key) == normalized)
			return &request.headers[i];
		++i;
	}
	return NULL;
}

}
}
