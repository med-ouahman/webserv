#pragma once

#include "Timestamp.hpp"
#include "Stream.hpp"
#include "IRequestHandler.hpp"

namespace http { class Contex; }
namespace net {

enum ConnectionState {
    READING,
    WRITING,
    CLOSING,
};

class Connection: public io::IStreamDelegate {
public:
    explicit Connection(int fd, io::Event events);
    ~Connection();
    bool timedout();
    void  update_stream();
    ConnectionState state() const;
    void consume(DataView& view);
    void produce(BufferWriter& writer);
    void on_stream_error();
    void on_stream_closed();
    io::Stream& stream();

private:
    io::Stream      stream_;
    ConnectionState state_;
    http::IRequestHandler* request_handler;

    bool            close_after_write;
    Timestamp       last_activity_;
    Timestamp       lifetime_;

    // http::Context   ctx;

};

}
