#include "CGIBodyProvider.hpp"

namespace http {

CGIBodyProvider::CGIBodyProvider(BufferReader& src)
    : source(src) {}

ssize_t CGIBodyProvider::produce(BufferWriter& writer) {
    size_t w = writer.write(source.data(), source.size());
    source.advance(w);
    return w;
}

}
