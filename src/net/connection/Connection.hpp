#pragma once

#include "Timestamp.hpp"
#include "Context.hpp"
#include "Stream.hpp"

namespace net {

enum ConnectionState {
    READING,
    WRITING,
    CLOSING,
};

class Connection: public io::IStreamDelegate {

public:
    explicit Connection( int fd, io::Event events );
    ~Connection();
    bool timedout();
    void  update_stream();
    ConnectionState state() const;
    void consume( DataView& view );
    void produce( BufferWriter& writer );
    void on_stream_error();
    void on_stream_closed();
    io::Stream& stream();

private:
    io::Stream      stream_;
    ConnectionState state_;
    
    bool            close_after_write;
    Timestamp       last_activity_;
    Timestamp       lifetime_;

    // http::Context   ctx;
};

}
