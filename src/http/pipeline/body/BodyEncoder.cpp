
#include "BodyEncoder.hpp"
#include <sstream>
#include <cstring>

#ifdef DEBUG
    #include <cassert>
#endif

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
    /*
        a\r\n
        data
        \r\n
    */
    switch (state_) {

        case Header: {

            
            write_ptr = writer.write_ptr();
            std::string overhead = std::string(header_overhead, '0');

            writer.write(overhead.c_str(), overhead.size());

            state_ = Data;
           
        }
        /* fall through */
        case Data: {
            
            ssize_t n = body->read(writer, writer.bytes_free());
            if (n < 0) return -1;

            current_chunk_ = n;
    
            std::string header = format(current_chunk_);
            ::memcpy(write_ptr, header.c_str(), header.size());
            ::memmove(write_ptr + header.size(), write_ptr + header_overhead, writer.length() - header_overhead);
            writer.pop(header_overhead - header.size());

            state_ = Trail;
           
        }
        /* fall through */
        case Trail:
            writer.write(trailer.c_str(), trailer.size());
            state_ = current_chunk_ == 0 ? Final: Header;
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

void ChunkedEncoder::reset() {
    state_ = Header;
    current_chunk_ = 0;
    write_ptr = NULL;
}


BodyEncoder::BodyEncoder()
    : encoding_(ContentLength) {
}

BodyEncoder::~BodyEncoder() {}


ssize_t BodyEncoder::encode(IBodyProvider* body, BufferWriter& writer) {

    switch (encoding_) {
        case Chunked:
            return chunked_.process(body, writer);
        case ContentLength: {

            if (fixed_.written >= fixed_.content_length) return 0;

            size_t rem = fixed_.content_length - fixed_.written;
            
            ssize_t n = body->read(writer, rem);
            
            if (n >= 0) fixed_.written += n;
            
            return n;
        }

        default:
            #ifdef DEBUG
                assert(false && "Unknown encoding type");
            #endif
    }
    
    return 0;
}

void BodyEncoder::reset(Encoding encoding) {
    encoding_ = encoding;
    chunked_.reset();
}

}
}