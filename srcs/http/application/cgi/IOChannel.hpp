#pragma once

#include "Stream.hpp"
#include "IDataListener.hpp"

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
            io::IDataListener* listener;
            STDStream::Type stream;
            uint32_t event;
        
        public:
            explicit IOChannel( int fd, CGIHandler* h, STDStream::Type stream_type, uint32_t event_mask );
            int get_fd() const;
            uint32_t get_event() const;
            ~IOChannel();
        
        private:
            bool process();
            void on_writeable();
            void on_readable();
            void on_error();
            bool readbuf_drained();
            void handle_event();
    };
}
