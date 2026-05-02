#include "LineScanner.hpp"

namespace http {

	LineScanner::LineScanner()
	: cr_found(false)
	{}

	LineScanner::~LineScanner() {}

	std::string const& LineScanner::line() {
		return linebuff;
	}

	void LineScanner::reset() {
		linebuff.clear();
		
		cr_found = false;
	}

	ScanResult LineScanner::scan( size_t max_block_len ) {

		size_t line_offset = linebuff.size();
		size_t i = view->bytes_consumed;

		bool nl_found = false;

		if (i == view->len_) {
			return NEED_MORE;
		}
		
		while (i < view->len_) {
			if (line_offset >= max_block_len) {
				return ERROR;
			}
			line_offset++;
			if (view->data_ptr_[i] == '\r') {
				++i;
				cr_found = true;
			} else if (view->data_ptr_[i] == '\n' && cr_found) {
				nl_found = true;
				cr_found = false;
				++i;
				break;
			} else {
				cr_found = false;
				++i;
			}
		}

		::size_t to_append = i - view->bytes_consumed - 1 * int(nl_found);
		linebuff.append(view->data_ptr_ + view->bytes_consumed, to_append);
		view->bytes_consumed = i;
		
		if (!nl_found) {
			return NEED_MORE;
		}
		
		linebuff.erase(linebuff.size() - 1, 1);
		return SUCCESS;
	}

}
