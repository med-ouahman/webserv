#include "LineReader.hpp"
#include <iostream>

namespace http {

LineReader::LineReader(): cr_found(false) {}

LineReader::~LineReader() {}

std::string const& LineReader::line() { return line_; }

void LineReader::reset() {
    line_.clear();
    cr_found = false;
}

ReadResult LineReader::readline(BufferReader& reader, size_t max_block_len) {
    size_t line_offset = line_.size();
    size_t i = reader.cursor();
    bool nl_found = false;

    if (i == reader.size()) return NEED_MORE;
    
    while (i < reader.size())
    {
        if (line_offset >= max_block_len) return LIMIT_EXCEEDED;
        
        line_offset++;

        char c = reader.data()[i];

        if (c == '\r') {
            cr_found = true;
            ++i;
        } else if (c == '\n' && cr_found) {
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
    
    if (to_append > 0) line_.append(reader.data() + reader.cursor(), to_append);
    
    reader.advance(to_advance);
    
    if (!nl_found) return NEED_MORE;
    
    if (line_[line_.size() - 1] == '\r') line_.erase(line_.size() - 1, 1);

    return SUCCESS;
}
}
