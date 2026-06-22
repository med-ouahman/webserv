#include "LineReader.hpp"
#include <iostream>

namespace http {

LineReader::LineReader(): cr_found(false) {}

LineReader::~LineReader() {}

std::string const& LineReader::line() {
    size_t crlf = line_.size() - 2;
    
    if (line_[crlf] == '\r' && line_[crlf + 1] == '\n') {
        line_.resize(crlf);
    }
    
    return line_;
}

void LineReader::reset() {
    line_.clear();
    cr_found = false;
}

ReadResult LineReader::readline(BufferView& reader, size_t max_block_len) {

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
    
    if (to_advance > 0) line_.append(reader.data() + reader.cursor(), to_advance);
    
    reader.advance(to_advance);
    
    if (!nl_found) return NEED_MORE;


    std::cout << line_ << "\n";

    std::cout << (line_[line_.size()-2] == '\r' ? "\\r   ":"NO\n") << (line_[line_.size()-1] == '\n' ? "\\n":"nono\n");
    return SUCCESS;
}
}
