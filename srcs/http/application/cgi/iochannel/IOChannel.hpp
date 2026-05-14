#pragma once

#include "Stream.hpp"
#include "IDataListener.hpp"
#include "DataView.hpp"

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

    class IOChannelState {
        public:
            enum Type {
                IDLE,
                ACTIVE,
                CLOSED,
                ERROR,
            };
    };

    class IOChannel: public io::Stream {

        private:
            CGIHandler& cgi_handler;
            STDStream::Type stream;
            uint32_t event;
            IOChannelState::Type state;
            
        public:
            explicit IOChannel( int fd, CGIHandler& h, STDStream::Type stream_type, uint32_t event_mask );
            int get_fd() const;
            uint32_t get_event() const;
            ~IOChannel();
            DataView& get_view();
            void process();
            void shutdown();
            std::string channel_type( STDStream::Type S );
            
        private:
            bool readbuf_drained();
            void handle_event();
            void on_read_eof();
            void on_read_error();
			void process_incoming_data();
			void process_outgoing_data();
            void on_write_complete();
            void on_write_error();
    };
}
