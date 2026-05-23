
#include "CGIRequestHandler.hpp"
#include "Parser.hpp"
#include <cstdlib>
#include "StatusCode.hpp"

namespace http {

	ParseResult CGIRequestHandler::parse_headers( DataView& view ) {
		

		while (ctx.state_ != HEADERS_DONE) {

			size_t max_scan_size = CGIParseContext::MAX_CGI_HEADER_BLOCK_LEN - ctx.header_bytes_;

			ctx.sc_.reset();
			ScanResult r = ctx.sc_.scan(view, max_scan_size);
			
			if (r == LIMIT_EXCEEDED) return PARSE_ERROR;
			if (r == NEED_MORE) return PARSE_CONTINUE;

			if (ctx.sc_.line().empty() && ctx.state_ == HEADERS) {
				ctx.state_ = HEADERS_DONE;
				break;
			}

			std::cout << ctx.sc_.line() << "\n";
			Base::Expected<std::pair<std::string, std::string>, int> header_result = Parser::parse_header(ctx.sc_.line());

			if (not header_result.has_value()) {
				std::cout  << "Error\n";
				return PARSE_ERROR;
			}

			std::cout << header_result.value().first << " | " << header_result.value().second << "\n";

			switch (ctx.state_) {
				case STATUS_LINE:
					sanitize_status_line(header_result.value());
					break;
				case HEADERS:
					std::cout << "XXD Headers\n";
					sanitize_header(header_result.value());
					break;
				default:
					return PARSE_SUCCESS;
			}
		}

		return PARSE_SUCCESS;
	}

	void CGIRequestHandler::sanitize_status_line( const std::pair<std::string, std::string>& header ) {
		if (header.first != "status") return;

		const std::string& value = header.second;

		size_t space_pos = value.find(' ');

		if (space_pos == std::string::npos) {
			ctx.state_ = CGI_ERROR;
			return ;
		}

		std::string code_str = value.substr(0, space_pos);
		std::string reason   = value.substr(space_pos + 1);

		for ( size_t i = 0; i < code_str.size(); ++i ) {
			if (!std::isdigit(code_str[i])) {
				ctx.state_ = CGI_ERROR;
				return;
			}
		}

		int code = std::atoi(code_str.c_str());

		if (code < MIN_HTTP_STATUS_CODE or code > MAX_HTTP_STATUS_CODE) {
			return ;
		}

		ctx.state_ = HEADERS;
		std::cout << "Done\n";
	}

	void CGIRequestHandler::sanitize_header( std::pair<std::string, std::string>& header ) {
		Parser::capitalize_http_header_name(header.first);
		headers_.add(header.first, header.second);
	}

	const Headers& CGIRequestHandler::headers() const {
		return headers_;
	}
}
