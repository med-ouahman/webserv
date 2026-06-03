
#include "Parser.hpp"

namespace http {
namespace parser {
bool validate_version(std::string const& s) {
	return s == "HTTP/1.1" || s == "HTTP/1.0";
}

void normalize_header_name(std::string& name) {
	for (size_t i = 0; i < name.length(); i++) {
		if (name[i] >= 'A' && name[i] <= 'Z') name[i] = tolower(name[i]);
	}
}

bool validate_header_name(const std::string& name) {
	for (size_t i = 0; i < name.length(); i++) {
		if (!isascii(name[i]) or !isprint(name[i]))	return false;
	}
	return true;
}

std::string capitalize_header_name(const std::string& name) {
	bool _cap = true;
	std::string capitalized;
	capitalized.reserve(name.size());
	for (size_t i(0); i < name.size(); ++i) {
		char c = name[i];
		if (_cap)
			c = ::toupper(c);
		capitalized.append(&c, 1);
		if (c == '-') _cap = true;
		else _cap = false;
	}
	return capitalized;
}

}
}
