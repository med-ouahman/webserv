#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include <sstream>

namespace http {

    CGIBodyProvider::CGIBodyProvider( CGIHandler& h, BodySendMethod::Type b, size_t size )
        : content_length(size),
        body_bytes_read(0),
        chunk_size(0),
        send_method(b),
        chunk_state(ChunkState::CHUNK_HEAD),
        cgi_handler(h),
        data_view(h.get_stdout_data_view()),
        temp_writer(NULL, 0)
    {}

    CGIBodyProvider::~CGIBodyProvider() {

    }

    bool CGIBodyProvider::finished() const {
        return true;
    }

    ssize_t CGIBodyProvider::read( BufferWriter* writer ) {

        switch (send_method) {
            case BodySendMethod::CHUNKED:
                return send_body_chunked(writer);

            case BodySendMethod::CONTENT_LENGTH:
                return send_body_content_length(writer);
        }

        assert(false && "UNDEFINED BODY TYPE!!!");
        return -1;
    }
    /*
        FF\r\n
        data
        \r\n
    */
   
    void CGIBodyProvider::format_chunk( size_t size ) {
        std::stringstream ss;
        ss << std::hex << size;

        chunk_header = ss.str();
        chunk_header.append("\r\n");
    }

   
}
