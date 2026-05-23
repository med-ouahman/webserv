#include "BodyProvider.hpp"
#include <cassert>
#include <sstream>

namespace http {


    BodyProvider::BodyProvider( const BodyContext& ctx )
        : source(ctx.source),
        type(ctx.type),
        body_content_length(ctx.body_content_length),
        chunk_state(CHUNK_HEAD),
        body_bytes_sent(0) {}

    BodyProvider::~BodyProvider() {}

       /*
        FF\r\n
        data
        \r\n
    */
   
    void BodyProvider::format_chunk( size_t size ) {
        std::stringstream ss;
        ss << std::hex << size;

        chunk_header = ss.str();
        chunk_header.append("\r\n");
    }

    ssize_t BodyProvider::read( BufferWriter& w ) {
       switch (type) {
            case CONTENT_LENGTH:
                return write_body_content_length(w);
            case CHUNKED:
                return write_body_chunked(w);
            default:
                #ifdef DEBUG
                    assert(false && "Unkonwn body type");
                #endif
                break;
       }
    }
}
