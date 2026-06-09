
#include "ChunkedEncoder.hpp"
#include <sstream>
#include <cstring>

namespace http {
namespace body {

const std::string ChunkedEncoder::trailer = "\r\n";

ChunkedEncoder::ChunkedEncoder()
: state_(HEADER),
current_chunk_(0),
write_ptr(NULL)
{}

ChunkedEncoder::~ChunkedEncoder() {

}

ssize_t ChunkedEncoder::process(IBodyProvider* body, BufferWriter& writer) {

    switch (state_) {

        case HEADER: {

            
            write_ptr = writer.write_ptr();
            std::string overhead = std::string(header_overhead, '0');

            writer.write(overhead.c_str(), overhead.size());

            state_ = DATA;
        }

        case DATA: {
            ssize_t n = body->read(writer);
            
            if (n < 0) return -1;
            
            if (n == 0)
                current_chunk_ = 0;
            
            else if (n < current_chunk_)
            {
                current_chunk_ = n;
                std::string header = format(current_chunk_);
                size_t new_size = header.size();
                ::memmove(write_ptr + new_size, write_ptr + old_size, writer.length() - old_size);
                writer.pop(old_size - new_size);
            }

            state_ = TRAIL;
        }

        case TRAIL:
            writer.write(trailer.c_str(), trailer.size());
            if (current_chunk_ == 0) state_ = FINAL;
            else state_ = HEADER;
        case FINAL:
            return 0;
    }

    return writer.length();
}


std::string ChunkedEncoder::format(size_t chunk) {
    std::stringstream ss;

    ss << std::hex << chunk;

    return ss.str() + "\r\n";    
}

}
}
