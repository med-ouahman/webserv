#include "BodyEncoder.hpp"

namespace http {
namespace body {

BodyEncoder::BodyEncoder(IBodyProvider* b, Encoding enc)
 : body(b),
 encoding_(enc) {
}

BodyEncoder::~BodyEncoder() {
    delete body;
    body = NULL;
}


ssize_t BodyEncoder::produce(BufferWriter& writer) {

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