#include "CGIBodyProvider.hpp"

namespace http {

CGIBodyProvider::CGIBodyProvider(BufferReader& src)
    : source(src) {}

CGIBodyProvider::~CGIBodyProvider() {

}

ssize_t CGIBodyProvider::read(BufferWriter& writer, size_t size) {
    size_t w = writer.write(source.data() + source.cursor(), std::min(size, source.remaining()));
    source.advance(w);
    return w;
}

}
