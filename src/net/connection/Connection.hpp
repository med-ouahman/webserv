#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "Timestamp.hpp"
#include "Limits.hpp"
#include "HttpSession.hpp"

namespace config {
    struct ServerConfig;
}

namespace net {

    class Connection: io::IStreamDelegate {
        
        public:
            explicit Connection( int fd, io::EventMask mask );
            ~Connection();
            bool timedout();
            ConnectionAction action() const;
            void on_stream_writeable();
            void on_stream_readable();
            void on_stream_error();

        private:
            io::Stream stream;

            ConnectionState state;
            bool            close_after_write;
            Timestamp       last_;
            Timestamp       conn_lifetime;
            http::HttpSession session;
        
    };
}
