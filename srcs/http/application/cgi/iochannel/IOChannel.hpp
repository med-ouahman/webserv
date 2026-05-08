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
            core::DataView& get_view();
            void process();
            // #ifdef DEBUG
            std::string channel_type( STDStream::Type S );
            // #endif
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
