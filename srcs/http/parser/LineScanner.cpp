#include "LineScanner.hpp"

namespace http {

	LineScanner::LineScanner()
	: cr_found(0)
	{}

	LineScanner::~LineScanner() {}

	std::string const& LineScanner::line() {
		return linebuff;
	}

	void LineScanner::reset() {
		linebuff.clear();
		
		cr_found = 0;
	}

	ScanResult LineScanner::scan( size_t max_block_len ) {

		size_t line_offset = linebuff.size();
		size_t i = bytes_consumed;

		bool nl_found = false;

		if (i == view.len_) {
			return NEED_MORE;
		}
		
		while (i < view.len_) {
			if (line_offset >= max_block_len) {
				return ERROR;
			}
			line_offset++;
			if (view.data[i] == '\r') {
				++i;
				cr_found = true;
			} else if (view.data[i] == '\n' && cr_found) {
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
		linebuff.append(view.data + bytes_consumed, to_append);
		bytes_consumed = i;
		
		if (!nl_found) {
			return NEED_MORE;
		}

		linebuff.erase(linebuff.size() - 1, 1);
		
		return SUCCESS;
	}

}
