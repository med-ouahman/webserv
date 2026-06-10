#include "BodyEncoder.hpp"

namespace http {
namespace body {

BodyEncoder::BodyEncoder(Encoding enc)
    : encoding_(enc) {
}

BodyEncoder::~BodyEncoder() {}


ssize_t BodyEncoder::produce(IBodyProvider* body, BufferWriter& writer) {

    switch (encoding_) {
        case Chunked:
            return chunked_.process(body, writer);
        case ContentLength:
            return body->read(writer);
        default:
            #ifdef DEBUG
            assert(false && "Unknown encoding type");
            #endif
    }
    
    return 0;
}

}
}