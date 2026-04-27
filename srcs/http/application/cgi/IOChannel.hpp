#pragma once

#include "IIOHandler.hpp"

namespace http {

    class CGIHandler;

    class Stream {
        public:
            enum Type {
                STDIN,
                STDOUT,
                STDERR,  
            };
    };

    class IOChannel: public io::IIOHandler {

        private:
            int fd;
            const CGIHandler* cgi_handler;
            Stream::Type stream;
            uint32_t event;
        
        public:
            void on_event( io::EventType event );
            explicit IOChannel( int fd, const CGIHandler* h, Stream::Type stream_type, uint32_t event_mask );
            int get_fd() const;
            uint32_t get_event() const;
            ~IOChannel();
    };
}
