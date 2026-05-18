#include "Parser.hpp"
#include "Expected.hpp"

#ifdef DEBUG
#include <iostream>
#endif

namespace http {
	
    ScanResult parser::parse_http_request( LineScanner& scanner, HTTPRequest& request ) {
		
		while (!request.finished()) {
			
			scanner.reset();

			size_t current_max_size = request.current_state() == RequestState::REQUEST_LINE
				? http_limits::MAX_REQUEST_LINE_LEN
				: http_limits::MAX_HEADER_BLOCK_LEN - request.total_header_bytes();
			
			ScanResult r = scanner.scan(current_max_size);
			
			if (r != SUCCESS) return r;

			switch (request.current_state()) {

				case RequestState::REQUEST_LINE: {	
					Base::Expected<RequestLine, int> rl_result = parse_request_line(scanner.line());
					if (!rl_result.has_value()) return static_cast<ScanResult>(rl_result.error());
					request.add_request_line(rl_result.value());
					
					break;
				}

				case RequestState::HEADERS: {	
					std::cout << "Parsing headesrs\n";
					if (scanner.line().empty()) {
						request.on_finished();
						break;
					}

					Base::Expected<std::pair<std::string, std::string>, int> header_result = parse_header(scanner.line());

					if (!header_result.has_value()) return static_cast<ScanResult>(header_result.error());
					
					request.add_request_header(header_result.value());
					break;
				}

				case RequestState::REQUEST_ERROR:
					return ERROR;
				default:
					break;
			}
		}
		
		return SUCCESS;
	}
}
