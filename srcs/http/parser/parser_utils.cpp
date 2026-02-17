
#include "Parser.hpp"

namespace http {
	bool Parser::validate_http_version( std::string const& s ) {
		if (s.length() < 5 || s.compare(0, 5, "HTTP/")) {
			return false;
		}
		std::string version = s.substr(5);
		std::cout << version << "\n";
		size_t i = 0;
		while (version[i] != '.' && i < version.length()) {
			if (!isdigit(version[i])) {
				return false;
			}
			i++;
		}
		if (i++ == version.length()) {
			return false;
		}
		while (i < version.length()) {
			if (!isdigit(version[i])) {
				return false;
			}
			i++;
		}
		if (i == version.length()) {
			return false;
		}
		return true;
	}
}
