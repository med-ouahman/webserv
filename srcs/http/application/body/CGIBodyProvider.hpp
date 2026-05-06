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

    class CGIBodyProvider: public IBodyProvider {

        public:
            bool    finished() const;
            ssize_t read( char* buff, size_t max_size );
            explicit CGIBodyProvider( CGIHandler& h, BodySendMethod::Type body_method, size_t size );
            ~CGIBodyProvider();

            

        private:
            size_t body_size;
            size_t body_bytes_read;

            BodySendMethod::Type send_method;
            CGIHandler& cgi_handler;
            core::DataView& data_view;
            CGIBodyProvider( const CGIBodyProvider& other );
            CGIBodyProvider& operator=( const CGIBodyProvider& other );
            std::string format_chunk( size_t chunk_size );

    };
}