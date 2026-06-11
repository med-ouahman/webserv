#pragma once

#include "IBodyProvider.hpp"

namespace http {
namespace body {

struct FixedEncoder {
    size_t content_length;
    size_t written;
};

class ChunkedEncoder {
public:

enum State {
    Header,
    Data,
    Trail,
    Final
};

private:
    const static std::size_t header_overhead = 10;
    const static std::string trailer;
    
    State       state_;
    size_t      current_chunk_;
    char*       write_ptr;
    
    static std::string format(size_t chunk_size);

public:
    ChunkedEncoder();
    ~ChunkedEncoder();
    ssize_t process(IBodyProvider* body, BufferWriter& w);
    void reset();
};


enum Encoding {
    Chunked,
    ContentLength,
};

class BodyEncoder {

private:

    Encoding        encoding_;
    ChunkedEncoder  chunked_;
    FixedEncoder    fixed_;

public:
    BodyEncoder();
    ~BodyEncoder();
    ssize_t encode(IBodyProvider* body, BufferWriter& writer);
    void reset(Encoding encoding);
};

}
}