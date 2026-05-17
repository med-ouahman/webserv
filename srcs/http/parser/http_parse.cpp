#include "HTTPParser.hpp"
#include "Expected.hpp"

namespace http {
	
    ScanResult HTTPParser::parse() {
		
		if (headers_done) return SUCCESS;

		if (ParseState::REQUEST_LINE == parse_state) {
			ScanResult r = line_c.scan(MAX_REQUEST_LINE_LEN);
			if (r != SUCCESS)
				return r;
			Base::Expected<RequestLine, int> result = parse_request_line(line_c.line());
			
			if (!result.has_value()) return ERROR;

			request.method = result.value().method;
			request.uri_path = result.value().uri;
			request.version = result.value().version;
			std::cout << "Request Line done\n";
			parse_state = ParseState::HEADERS;
			line_c.reset();
		}
		
		if (ParseState::HEADERS == parse_state) {

			ScanResult r = line_c.scan(MAX_HEADER_BLOCK_LEN);
			
			if (r != SUCCESS) return r;
			
			if (line_c.line().empty()) {
				headers_done = true;
				if (!validate_headers()) return ERROR;
				return SUCCESS;
			}

			++header_count;
			
			if (header_count > MAX_HEADER_COUNT) return ERROR;

			Base::Expected<std::pair<std::string, std::string>, int> header = parse_headers(line_c.line());
			request.headers[header->first] = header->second;
			line_c.reset();
		}

		if (ParseState::ERROR == parse_state) return ERROR;

		return SUCCESS;
	}
}
