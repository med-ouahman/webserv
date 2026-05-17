
#include "Parser.hpp"

namespace http {
	
	bool Parser::validate_http_version( std::string const& s ) {
		return s == "HTTP/1.1" || s == "HTTP/1.0";
	}

	void Parser::normalize_http_header_name( std::string& name ) {
		for ( size_t i = 0; i < name.length(); i++ ) {
			if (name[i] >= 'A' && name[i] <= 'Z') name[i] = tolower(name[i]);
		}
	}

	bool Parser::validate_http_header_name( const std::string& name ) {

		for ( size_t i = 0; i < name.length(); i++ ) {
			if (!isascii(name[i]) or !isprint(name[i]))	return false;
		}

		return true;
	}


}
