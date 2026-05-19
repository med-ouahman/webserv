
#include "Parser.hpp"

namespace http {
	
	bool parser::validate_http_version( std::string const& s ) {
		return s == "HTTP/1.1" || s == "HTTP/1.0";
	}

	void parser::normalize_http_header_name( std::string& name ) {
		for ( size_t i = 0; i < name.length(); i++ ) {
			if (name[i] >= 'A' && name[i] <= 'Z') name[i] = tolower(name[i]);
		}
	}

	bool parser::validate_http_header_name( const std::string& name ) {

		for ( size_t i = 0; i < name.length(); i++ ) {
			if (!isascii(name[i]) or !isprint(name[i]))	return false;
		}

		return true;
	}


	void parser::capitalize_http_header_name( std::string& name ) {

		bool cap = true;

		size_t pos = 0;
		
		while (pos < name.size()) {
			
			if (not cap && (name[pos] == '-')) {
				cap = true;
				++pos;
				continue;
			}

			if (cap) {
				name[pos] = std::toupper(name[pos]);
				cap = false;
			}
			
			++pos;
		}
	}

}
