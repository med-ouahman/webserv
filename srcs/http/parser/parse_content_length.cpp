
#include "BodyParser.hpp"

namespace http {

    bool BodyParser::parse_content_length( std::string const& s ) {
		for ( ::size_t i = 0; i < s.length(); i++ ) {
			if (!isdigit(s[i])) {
				return false;
			}
			
			body_len = body_len * 10 + s[i] - 48;
			if (body_len > 101010) {
				return false;
			}
		}

		return true;
	}
}
