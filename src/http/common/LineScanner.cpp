#include "LineScanner.hpp"
#include <iostream>

namespace http {

	LineScanner::LineScanner()
:
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

    ScanResult LineScanner::scan(BufferReader& reader, size_t max_block_len) {
        size_t line_offset = linebuff.size();
        size_t i = reader.cursor();
        bool nl_found = false;

        if (i == reader.size()) {
            return NEED_MORE;
        }
        
        while (i < reader.size()) {
            if (line_offset >= max_block_len) {
                return LIMIT_EXCEEDED;
            }
            line_offset++;

            char current_char = reader.data()[i];

            if (current_char == '\r') {
                cr_found = true;
                ++i;
            } else if (current_char == '\n' && cr_found) {
                nl_found = true;
                cr_found = false;
                ++i;
                break;
            } else {
                cr_found = false;
                ++i;
            }
        }
       
        size_t to_advance = i - reader.cursor();
        size_t to_append = to_advance - (nl_found ? 1 : 0);
        
        if (to_append > 0) {
            linebuff.append(reader.data() + reader.cursor(), to_append);
        }
        
        reader.advance(to_advance);
        
        if (!nl_found) return NEED_MORE;
        
        if (linebuff[linebuff.size() - 1] == '\r') linebuff.erase(linebuff.size() - 1, 1);
    
        return SUCCESS;
    }
}
