#pragma once

#include "BufferWriter.hpp"
#include "IBodyProvider.hpp"

namespace http {
namespace body {

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
};


}

}
