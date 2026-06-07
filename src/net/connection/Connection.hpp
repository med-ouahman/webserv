#pragma once

#include "Registrar.hpp"
#include "Timestamp.hpp"
#include "Stream.hpp"
#include "Context.hpp"

namespace net {

enum ConnectionState {
    READING,
    WRITING,
    CLOSING,
};

class Connection: public io::IStreamDelegate {
    
public:
    Connection(int fd, io::Event events, RegisterContext& ctx);
    ~Connection();
    bool timedout();
    void  update_stream();
    ConnectionState state() const;
    void consume(BufferReader& view);
    void produce(BufferWriter& writer);
    void on_stream_error();
    void on_stream_closed();
    io::Stream& stream();
    
private:
    io::Stream      stream_;
    ConnectionState state_;    
    bool            close_after_write;
    Timestamp       last_activity_;
    Timestamp       lifetime_;

    RegisterContext register_ctx;

    http::Context   ctx;
};

}
