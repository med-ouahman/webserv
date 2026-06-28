
#include "BodyEncoder.hpp"
#include <sstream>
#include <cstring>
#include <iostream>

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

ssize_t ChunkedEncoder::process(IBodyProvider* body, std::string& out) {
    ssize_t n = body->read(out, out.capacity());

    if (n < 0) return n;
    
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


/* the default is Chunked unless specified otherwise */
BodyEncoder::BodyEncoder()
    : encoding_(Chunked),
    fixed_(0) {
}


/* Use this for Content Length */
BodyEncoder::BodyEncoder(size_t content_length)
    : encoding_(ContentLength),
    fixed_(content_length) {
}


BodyEncoder::~BodyEncoder() {}


const std::string& BodyEncoder::encode(IBodyProvider* body) {

    switch (encoding_) {
        case Chunked:
            chunked_.process(body, chunk_);
        case ContentLength: {

            std::cout << "Content-Length: " << fixed_.content_length << "\n";
            if (fixed_.written >= fixed_.content_length) return 0;

            size_t rem = fixed_.content_length - fixed_.written;
            
            ssize_t n = body->read(chunk_, rem);
            
            if (n >= 0) fixed_.written += n;
        }
    }
    
    return chunk_;
}

}
}