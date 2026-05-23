#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "Timestamp.hpp"
#include "limits.hpp"
#include "Stream.hpp"
#include "HttpSession.hpp"

namespace config {
    struct ServerConfig;
}

namespace net {

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

}
