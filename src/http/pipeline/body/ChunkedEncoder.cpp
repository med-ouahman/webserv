
#include "ChunkedEncoder.hpp"
#include <sstream>
#include <cstring>

namespace http {
namespace body {

const std::string ChunkedEncoder::trailer = "\r\n";

ChunkedEncoder::ChunkedEncoder()
: state_(Header),
current_chunk_(0),
write_ptr(NULL)
{}

ChunkedEncoder::~ChunkedEncoder() {

}

ssize_t ChunkedEncoder::process(IBodyProvider* body, BufferWriter& writer) {

    switch (state_) {

        case Header: {

            
            write_ptr = writer.write_ptr();
            std::string overhead = std::string(header_overhead, '0');

            writer.write(overhead.c_str(), overhead.size());

            state_ = Data;
        }

        case Data: {
            
            ssize_t n = body->read(writer);
            if (n < 0) return -1;

            current_chunk_ = n;
    
            std::string header = format(current_chunk_);
            ::memcpy(write_ptr, header.c_str(), header.size());
            ::memmove(write_ptr + header.size(), write_ptr + header_overhead, writer.length() - header_overhead);
            writer.pop(header_overhead - header.size());

            state_ = Trail;
        }

        case Trail:
            writer.write(trailer.c_str(), trailer.size());
            if (current_chunk_ == 0) state_ = Final;
            else state_ = Header;
            break;
        case Final:
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
