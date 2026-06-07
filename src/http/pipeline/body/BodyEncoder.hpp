#pragma once

#include "IBodyProvider.hpp"
#include <string>
namespace http {

class ChunkedEncoder {
private:
    const static std::size_t max_chunk_header = 12;
    enum ChunkState {
        HEAD,
        DATA,
        TRAIL,
        LAST,
    } state_;

    size_t remaining_;
    std::string formatted;

public:
    std::string& format_chunk(size_t chunk_size);
    ssize_t write(BufferWriter& w);
};

struct ContentLengthEncoder {
    size_t content_length;
    size_t sent_;

};

class BodyEncoder {
public:

enum Encoding {
    CONTENT_LENGTH,
    CHUNKED,
};

private:
    bool can_read;
    IBodyProvider* body_;
    Encoding encoding_;
    ChunkedEncoder chunked_;
    ContentLengthEncoder lengthed_;

    BodyEncoder(const BodyEncoder&);
    BodyEncoder& operator=(const BodyEncoder&);

public:
    BodyEncoder(IBodyProvider* b, Encoding e);
    ~BodyEncoder();
    ssize_t read(BufferWriter& w);

};

}
