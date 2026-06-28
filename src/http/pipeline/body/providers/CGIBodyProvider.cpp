
#include "CGIBodyProvider.hpp"
#include "CgiHandler.hpp"

namespace http {

CGIBodyProvider::CGIBodyProvider(CgiHandler& h, BufferView& src)
    : handler(h),
    source(src) {}

CGIBodyProvider::~CGIBodyProvider() {}

ssize_t CGIBodyProvider::read(std::string& out, size_t size) {
    
    size_t av = std::min(source.size(), size);

    ::memcpy(&out[0], source.data(), av);

    source.advance(av);

    return av;
}

}
