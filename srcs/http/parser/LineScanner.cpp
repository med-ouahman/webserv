#include "LineScanner.hpp"

namespace http {

	LineScanner::LineScanner( DataView& v )
	:data_view(v),
	cr_found(false)
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
		size_t i = data_view.cursor();

		bool nl_found = false;

		if (i == data_view.size()) {
			return NEED_MORE;
		}
		
		while (i < data_view.size()) {
			if (line_offset >= max_block_len) {
				return ERROR;
			}
			line_offset++;
			if (data_view.data()[i] == '\r') {
				++i;
				cr_found = true;
			} else if (data_view.data()[i] == '\n' && cr_found) {
				nl_found = true;
				cr_found = false;
				++i;
				break;
			} else {
				cr_found = false;
				++i;
			}
		}

		::size_t to_append = i - data_view.cursor() - 1 * int(nl_found);
		linebuff.append(data_view.data() + data_view.cursor(), to_append);
		data_view.advance(i);
		
		if (!nl_found) {
			return NEED_MORE;
		}
		
		linebuff.erase(linebuff.size() - 1, 1);
		return SUCCESS;
	}

}
