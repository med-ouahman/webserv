
#include "CGIBodyProvider.hpp"
#include "CgiHandler.hpp"

namespace http {

CGIBodyProvider::CGIBodyProvider(CgiHandler& h, Buffer& src)
    : handler(h),
    source(src) {}

CGIBodyProvider::~CGIBodyProvider() {

}

ssize_t CGIBodyProvider::read(std::string& out, size_t size) {
    
    size_t av = std::min(source.bytes_pending(), size);

    ::memcpy(&out[0], source.read_ptr(), av);

    source.advance_read(av);

    return av;
}

}
