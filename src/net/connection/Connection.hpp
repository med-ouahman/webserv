#pragma once

#include "Timestamp.hpp"
#include "Context.hpp"
#include "Stream.hpp"

namespace config {
    struct ServerConfig;
}

namespace net {

enum ConnectionState {
    READING,
    WRITING,
    CLOSING,
};

enum ConnectionAction {
    READ,
    WRITE,
    CLOSE,
};

struct ServerContext {
    void* context;
};

class Connection;

typedef void (*DisconnectCallback)( ServerContext ctx, Connection* );

class Connection: public io::IStreamDelegate {

public:
    explicit Connection( int fd, io::Event events, ServerContext server_ctx, DisconnectCallback cb_ );
    ~Connection();
    bool timedout();
    void  update_stream();
    void consume( DataView& view );
    void produce( BufferWriter& writer );
    void on_stream_error();
    void on_stream_closed();
    io::Stream& stream();

private:
    io::Stream      stream_;
    ConnectionState state;

    ServerContext   server;
    DisconnectCallback disconnect_;
    
    bool            close_after_write;
    Timestamp       last_activity_;
    Timestamp       lifetime_;

    // http::Context   ctx;
};

}
