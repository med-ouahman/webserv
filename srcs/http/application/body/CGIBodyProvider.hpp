#include "IBodyProvider.hpp"
#include "DataView.hpp"
#include <cassert>
#include <string>
#include "BufferWriter.hpp"

namespace http {

    class CGIHandler;

    struct BodySendMethod {
        enum Type {
            CONTENT_LENGTH,
            CHUNKED,
        };
    };


    class CGIBodyProvider: public IBodyProvider {

        public:
            bool    finished() const;
            ssize_t read( BufferWriter* writer );
            explicit CGIBodyProvider( CGIHandler& h, BodySendMethod::Type body_method, size_t size );
            ~CGIBodyProvider();

            struct ChunkState {
                enum Type {
                    NONE,
                    CHUNK_HEAD,
                    CHUNK_DATA,
                    CHUNK_TRAIL,
                    DONE,
                };
            };

        private:
            size_t content_length;
            size_t body_bytes_read;

            size_t chunk_size;
            std::string chunk_header;

            BodySendMethod::Type send_method;
            ChunkState::Type chunk_state;
            CGIHandler& cgi_handler;
            DataView& data_view;
            BufferWriter temp_writer;

            CGIBodyProvider( const CGIBodyProvider& other );
            CGIBodyProvider& operator=( const CGIBodyProvider& other );
            
            void format_chunk( size_t chunk_size );
            ssize_t send_body_content_length( BufferWriter* writer );
            ssize_t send_body_chunked( BufferWriter* writer );

            void chunk_head();
            void chunk_data();
            void chunk_trail();

    };
}