#include "HTTPParser.hpp"

namespace http {
    HTTPParser::ParseResult::Type HTTPParser::scan_line( ::size_t max_bytes_allowed ) {
		::size_t line_offset = line_buff.size();
		::size_t i = bytes_consumed;
		bool nl_found = false;

		if (i == len_) {
			return ParseResult::NEED_MORE_BYTES;
		}
		
		while (i < len_) {
			if (line_offset >= max_bytes_allowed) {
				return ParseResult::PARSE_ERROR;
			}
			line_offset++;
			if (data_[i] == '\r') {
				++i;
				cr_found = true;
			} else if (data_[i] == '\n' && cr_found) {
				nl_found = true;
				cr_found = false;
				++i;
				break;
			} else {
				cr_found = false;
				++i;
			}
		}		
		::size_t to_append = i - bytes_consumed - 1 * nl_found;
		line_buff.append(data_ + bytes_consumed, to_append);
		bytes_consumed = i;
		if (!nl_found) {
			return ParseResult::NEED_MORE_BYTES;
		}
		line_buff.erase(line_buff.size() - 1, 1);
		
		return ParseResult::SUCCESS;
	}
}