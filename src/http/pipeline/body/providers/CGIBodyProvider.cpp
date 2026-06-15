
#include "CGIBodyProvider.hpp"
#include "CgiHandler.hpp"

namespace http {

CGIBodyProvider::CGIBodyProvider(CgiHandler& h, BufferReader& src)
    : handler(h),
    source(src) {}

CGIBodyProvider::~CGIBodyProvider() {}

CGIBodyProvider::ReadResult CGIBodyProvider::read(BufferWriter& writer, size_t size) {

    if (source.empty()) state_ = Filling;
    else state_ = Draining;

    switch (state_) {
        case Filling:
            handler.resume_channel(Channel::Stdout);
            return Unavailabe;
            break;
        case Draining: {   
            size_t w = writer.write(source.data() + source.cursor(), std::min(size, source.remaining()));
            source.advance(w);
            return Success;
        }
    }

    return Success;
}

}
