#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "Timestamp.hpp"
#include "limits.hpp"
<<<<<<< HEAD
#include "Stream.hpp"
#include "HttpSession.hpp"
=======
#include "http/session/HttpSession.hpp"
#include "Stream.hpp"
>>>>>>> 2a4fb87 (s)

namespace config {
    struct ServerConfig;
}

namespace net {

<<<<<<< HEAD
enum ConnectionState {
    READING,
    WRITING,
    CLOSING,
};

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
    const io::Stream& stream() const;

private:
    io::Stream stream_;

    ConnectionState state_;
    bool            close_after_write_;
    Timestamp       last_activity_;
    Timestamp       conn_lifetime_;
    
};

=======
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
            const io::Stream& stream() const;

        private:
            io::Stream stream_;

            ConnectionState state;
            bool            close_after_write;
            Timestamp       last_;
            Timestamp       conn_lifetime;
            http::HttpSession session;
        
    };
>>>>>>> 2a4fb87 (s)
}
