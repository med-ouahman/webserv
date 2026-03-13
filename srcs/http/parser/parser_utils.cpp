
#include "HTTPParser.hpp"

namespace http {
	
	bool HTTPParser::validate_http_version( std::string const& s ) {
		return s == "HTTP/1.1" || s == "HTTP/1.0";
	}

	bool HTTPParser::parse_content_length( std::string const& s, size_t& body_len ) {
		for ( size_t i = 0; i < s.length(); i++ ) {
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

	bool HTTPParser::add_request_header( std::string const& s ) {
		::size_t colon_index = s.find(":");
		if (colon_index == std::string::npos) {
			std::cout << "colon not found\n";
			return false;
		}
		::size_t start = 0;
		while (iswspace(s[start]) && start < colon_index - 1) {
			start++;
		}
		::size_t end = colon_index - 1;
		while (iswspace(s[end])) {
			end--;
		}
		std::string name = s.substr(start, end + 1);
		if (!name.length()) {
			std::cout << "missing header name\n";
			return false;
		}
		HTTPParser::normalize_header_name(name);
		start = colon_index + 1;
		while (iswspace(s[start]) && start < s.length()) {
			start++;
		}
		end = s.length() - 1;
		while (iswspace(s[end])) {
			end--;
		}
		std::string value = s.substr(start, end + 1);
		request.headers[name] = value;
		return true;
	}

	bool HTTPParser::validate_headers( void ) {
		if (request.version == "HTTP/1.1") {
			if (request.headers["host"].length() == 0) {
				std::cout << "host not found\n";
				return false;
			}
		}
	
		std::string content_length = request.headers["content-length"];

		if (content_length.length() != 0) {
			if (!parse_content_length(content_length, request.body_len)) {
				return false;
			}
		}

		return true;
	}

	void HTTPParser::normalize_header_name( std::string& name ) {
		for ( ::size_t i = 0; i < name.length(); i++ ) {
			if (name[i] >= 'A' && name[i] <= 'Z') {
				name[i] = tolower(name[i]);
			}
		}
	}

	bool HTTPParser::validate_header_name( const std::string& name ) {

		for ( ::size_t i = 0; i < name.length(); i++ ) {
			if (!isascii(name[i]) || !isprint(name[i])) {				
				return false;
			}
		}
		return true;
	}
}
