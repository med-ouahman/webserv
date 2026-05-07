#include "IBodyProvider.hpp"
#include "DataView.hpp"

namespace http {

    class CGIHandler;

    struct BodySendMethod {
        enum Type {
            CONTENT_LENGTH,
            CHUNKED,
        };
    };

    struct ChunkState {
        enum Type {
            NONE,
            CHUNK_HEAD,
            CHUNK_DATA,
            CHUNK_TRAIL
        };
    };

    class CGIBodyProvider: public IBodyProvider {

        private:
            const static std::size_t MIN_CHUNK_SIZE = 1024 * 8; // 8KB

        public:
            bool    finished() const;
            ssize_t read( core::BufferWriter* writer, size_t max_size );
            explicit CGIBodyProvider( CGIHandler& h, BodySendMethod::Type body_method, size_t size );
            ~CGIBodyProvider();

        private:
            size_t content_length;
            size_t body_bytes_read;
            size_t chunk_size;
            std::string chunk_header;

            BodySendMethod::Type send_method;
            ChunkState::Type chunk_state;
            CGIHandler& cgi_handler;
            core::DataView& data_view;

            CGIBodyProvider( const CGIBodyProvider& other );
            CGIBodyProvider& operator=( const CGIBodyProvider& other );
            
            void format_chunk( size_t chunk_size );
            ssize_t send_body_content_length( core::BufferWriter* writer );
            ssize_t fill_buff( core::BufferWriter* writer );
            ssize_t send_body_chunked( core::BufferWriter* writer );
            ssize_t pull();

    };
}