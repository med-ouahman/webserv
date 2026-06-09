
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
            current_chunk_ = writer.bytes_free() - trailer.size();
            
            format(current_chunk_);
            write_ptr = writer.write_ptr();

            writer.write(header_.c_str(), header_.size());
            current_chunk_ -= header_.size();
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
                size_t old_size = header_.size();
                format(current_chunk_);
                size_t new_size = header_.size();
                ::memmove(write_ptr + new_size, write_ptr + old_size, writer.length() - old_size);
            }

            state_ = TRAIL;
        }

        case TRAIL:
            writer.write(trailer.c_str(), trailer.size());
        case FINAL:
           return 0;     
    }

    return writer.length();
}


void ChunkedEncoder::format(size_t chunk) {
    std::stringstream ss;

    ss << std::hex << chunk;

    header_ = ss.str() + "\r\n";    
}

}
}
