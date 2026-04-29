#pragma once

#include "Stream.hpp"

namespace http {

    class CGIHandler;

    class STDStream {
        public:
            enum Type {
                STDIN,
                STDOUT,
                STDERR,  
            };
    };

    class IOChannel: public io::Stream {

        private:
            const CGIHandler* cgi_handler;
            STDStream::Type stream;
            uint32_t event;
        
        public:
            explicit IOChannel( int fd, const CGIHandler* h, STDStream::Type stream_type, uint32_t event_mask );
            int get_fd() const;
            uint32_t get_event() const;
            ~IOChannel();
        
        private:
            bool process( void );
            void error( void );
            void on_writeable( void );
            void on_readable( void );
            bool readbuf_drained( void ) { return false; }
            void handle_event( void );
    };
}
