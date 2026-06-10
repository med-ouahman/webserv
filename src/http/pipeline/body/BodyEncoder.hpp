#pragma once

#include "IBodyProvider.hpp"
#include "ChunkedEncoder.hpp"

namespace http {
namespace body {

enum Encoding {
    Chunked,
    ContentLength,
};

class BodyEncoder {

private:
    const Encoding encoding_;
    ChunkedEncoder chunked_;

public:
    BodyEncoder(Encoding enc);
    ~BodyEncoder();
    ssize_t produce(IBodyProvider* body, BufferWriter& writer);
};

}
}