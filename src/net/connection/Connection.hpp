#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "Timestamp.hpp"
#include "limits.hpp"
#include "HttpSession.hpp"
#include "Stream.hpp"

namespace config {
    struct ServerConfig;
}

namespace net {

    class Connection: public io::IStreamDelegate {
        
        public:
            explicit Connection( int fd, io::EventMask mask );
            ~Connection();
            bool timedout();
            ConnectionAction action() const;
            void consume( DataView& view );
            void produce( BufferWriter& writer );
            void on_stream_error();
            void on_stream_closed();

        private:
            io::Stream stream;

            ConnectionState state;
            bool            close_after_write;
            Timestamp       last_;
            Timestamp       conn_lifetime;
            http::HttpSession session;
        
    };
}
