#pragma once

#include "BufferWriter.hpp"
#include "IBodyProvider.hpp"

namespace http {
namespace body {

class ChunkedEncoder {
public:

enum State {
    HEADER,
    DATA,
    TRAIL,
    FINAL
};

private:
    const static std::string trailer;
    State       state_;
    std::string header_;
    size_t      current_chunk_;
    char*       write_ptr;
    
    void format(size_t chunk_size);

    public:
    ChunkedEncoder();
    ~ChunkedEncoder();
    ssize_t process(IBodyProvider* body, BufferWriter& w);
};


}

}
