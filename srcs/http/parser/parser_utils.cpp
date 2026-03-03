
#include "HTTPParser.hpp"
#include <algorithm>

namespace http {
	bool HTTPParser::validate_http_version( std::string const& s ) {
		if (s.length() < 5 || s.compare(0, 5, "HTTP/")) {
			return false;
		}
		std::string version = s.substr(5);
		size_t i = 0;
		while (version[i] != '.' && i < version.length()) {
			if (!isdigit(version[i])) {
				return false;
			}
			i++;
		}
		if (i == version.length()) {
			return false;
		}
		i++;
		while (i < version.length()) {
			if (!isdigit(version[i])) {
				return false;
			}
			i++;
		}
		if (i != version.length()) {
			return false;
		}
		return true;
	}

	bool HTTPParser::parse_content_length( std::string const& s, size_t& body_len ) {
		for ( size_t i = 0; i < s.length(); i++ ) {
			if (!isdigit(s[i])) {
				return false;
			}
			body_len = body_len * 10 + s[i] - 48;
			if (body_len > BODY_MAX_LENGTH) {
				return false;
			}
		}
		return true;
	}

	bool HTTPParser::add_request_header( std::string const& s ) {
		size_t colon_index = s.find(":");
		if (colon_index == std::string::npos) {
			return false;
		}
		size_t start = 0;
		while (iswspace(s[start]) && start < colon_index - 1) {
			start++;
		}
		size_t end = colon_index - 1;
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
		for ( size_t i = 0; i < name.length(); i++ ) {
			if (name[i] >= 'A' && name[i] <= 'Z') {
				name[i] = tolower(name[i]);
			}
		}
	}
}
