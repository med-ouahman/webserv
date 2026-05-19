
#include "CGIRequestHandler.hpp"
#include "Parser.hpp"
#include <cstdlib>

namespace http {

	ScanResult CGIRequestHandler::parse_cgi_headers() {
		

		while (not parse_ctx.finished and parse_ctx.state not_eq CGIOutputState::ERROR) {

			size_t max_scan_size = MAX_CGI_HEADER_BLOCK_LEN - parse_ctx.bytes_parsed;

			scanner.reset();
			ScanResult r = scanner.scan(max_scan_size);
			
			if (r not_eq SUCCESS) return r;

			if (scanner.line().empty() and parse_ctx.state == CGIOutputState::HEADERS) {
				parse_ctx.finished = true;
				parse_ctx.state = CGIOutputState::BIND_BODY;
				break;
			}

			std::cout << scanner.line() << "\n";
			Base::Expected<std::pair<std::string, std::string>, int> header_result = parser::parse_header(scanner.line());

			if (not header_result.has_value()) {
				std::cout  << "Error\n";
				return ERROR;
			}

			std::cout << header_result.value().first << " | " << header_result.value().second << "\n";

			switch (parse_ctx.state) {
				case CGIOutputState::STATUS_LINE:
					sanitize_status_line(header_result.value());
					break;
				case CGIOutputState::HEADERS:
					std::cout << "XXD Headers\n";
					sanitize_header(header_result.value());
					break;
				case CGIOutputState::ERROR:
					return ERROR;
				default:
					return SUCCESS;
			}
		}

		return SUCCESS;
	}

	void CGIRequestHandler::sanitize_status_line( const std::pair<std::string, std::string>& header ) {
		if (header.first not_eq "status") return;

		const std::string& value = header.second;

		size_t space_pos = value.find(' ');

		if (space_pos == std::string::npos) {
			parse_ctx.status_code = INTERNAL_SERVER_ERROR;
			parse_ctx.status_reason = "Invalid Status";
			parse_ctx.state = CGIOutputState::ERROR;
			return ;
		}

		std::string code_str = value.substr(0, space_pos);
		std::string reason   = value.substr(space_pos + 1);

		for ( size_t i = 0; i < code_str.size(); ++i ) {
			if (!std::isdigit(code_str[i])) {
				parse_ctx.status_code = INTERNAL_SERVER_ERROR;
				parse_ctx.status_reason = "Invalid Status Code";
				parse_ctx.state = CGIOutputState::ERROR;
				return;
			}
		}

		int code = std::atoi(code_str.c_str());

		if (code < MIN_HTTP_STATUS_CODE or code > MAX_HTTP_STATUS_CODE) {
			parse_ctx.status_code = INTERNAL_SERVER_ERROR;
			parse_ctx.status_reason = "Invalid Status Code";
			cgi_state = CGIState::ERROR;
			return ;
		}

		parse_ctx.status_code  = static_cast<StatusCode>(code);
		parse_ctx.status_reason = reason;
		parse_ctx.state = CGIOutputState::HEADERS;
		std::cout << "Done\n";
	}

	void CGIRequestHandler::sanitize_header( std::pair<std::string, std::string>& header ) {
		parser::capitalize_http_header_name(header.first);
		parse_ctx.headers.add(header.first, header.second);
	}

	const Headers& CGIRequestHandler::cgi_headers() const {
		return parse_ctx.headers;
	}
}
