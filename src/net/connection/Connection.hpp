#pragma once

#include "ConnectionState.hpp"
#include "ConnectionAction.hpp"
#include "Timestamp.hpp"
#include "limits.hpp"
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
    http::Context   ctx;
};
}
