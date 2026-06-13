#pragma once

#include "IBodyProvider.hpp"

namespace http {
namespace body {

struct FixedEncoder {
    size_t content_length;
    size_t written;

    FixedEncoder(size_t length): content_length(length) {};
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

/*
    BodyEncoder: Encodes the body to be sent to the client
    @Content-Length: used when a content length is present
    @Chunked: used as a default encoder
*/

class BodyEncoder {
private:
    Encoding        encoding_;
    ChunkedEncoder  chunked_;
    FixedEncoder    fixed_;

public:
    BodyEncoder();
    BodyEncoder(size_t content_length);
    ~BodyEncoder();
    ssize_t encode(IBodyProvider* body, BufferWriter& writer);
};

}
}