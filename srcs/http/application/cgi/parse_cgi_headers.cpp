
#include "CGIHandler.hpp"
#include "Parser.hpp"
#include <cstdlib>

namespace http {

	ScanResult CGIHandler::parse_cgi_headers() {
		

		while (!parse_ctx.finished && parse_ctx.state != CGIOutputState::ERROR) {

			size_t max_scan_size = MAX_CGI_HEADER_BLOCK_LEN - parse_ctx.bytes_parsed;

			ScanResult r = scanner.scan(max_scan_size);
			
			if (r != SUCCESS) return r;

			if (scanner.line().empty() and parse_ctx.state == CGIOutputState::HEADERS) {
				parse_ctx.finished = true;
				parse_ctx.state = CGIOutputState::WRITING_BODY;
				break;
			}

			Base::Expected<std::pair<std::string, std::string>, int> header_result = parser::parse_header(scanner.line());

			if (not header_result.has_value()) return ERROR;

			switch (parse_ctx.state) {
				case CGIOutputState::STATUS_LINE:
					sanitize_status_line(header_result.value());
					break;
				case CGIOutputState::HEADERS: {
					sanitize_header(header_result.value());
					break;
				case CGIOutputState::ERROR:
					return ERROR;
				default:
					return SUCCESS;
				}
			}
		}

		return SUCCESS;
	}

	void CGIHandler::sanitize_status_line( const std::pair<std::string, std::string>& header ) {
		if (header.first != "status")
			return;

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

		parse_ctx.status_code  = static_cast<HTTPStatusCode>(code);
		parse_ctx.status_reason = reason;
		parse_ctx.state = CGIOutputState::HEADERS;
	}

	void CGIHandler::sanitize_header( std::pair<std::string, std::string>& header ) {
		parser::capitalize_http_header_name(header.first);
		parse_ctx.headers.push_back(header);
	}
}
