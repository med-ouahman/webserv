
#include "CGIBodyProvider.hpp"
#include "CgiHandler.hpp"

namespace http {

CGIBodyProvider::CGIBodyProvider(CgiHandler& h, BufferView& src)
    : handler(h),
    source(src) {}

CGIBodyProvider::~CGIBodyProvider() {

}

ssize_t CGIBodyProvider::read(BufferWriter& writer, size_t size) {
    
    size_t w = writer.write(source.data() + source.cursor(), std::min(size, source.remaining()));
    source.advance(w);
    return w;
}

}
