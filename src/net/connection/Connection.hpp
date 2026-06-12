#pragma once

#include "AEventHandler.hpp"
#include "Timestamp.hpp"
#include "Context.hpp"
#include "BufferReader.hpp"
#include "BufferWriter.hpp"

namespace net {

enum ConnectionState {
    Reading,
    Writing,
    Closing,
};

class Connection: public io::AEventHandler {
    
public:
    const static std::size_t ReadbufSize = 1024 * 4;
    const static std::size_t WritebufSize = 1024 * 4;
    Connection(int fd, io::Event events, ServerContext& ctx);
    ~Connection();

    void on_event(io::Event events);
    
    ConnectionState state() const;
    bool closing() const;

private:
    ConnectionState state_;  
    bool            close_after_write;

    Timestamp       last_activity_;
    Timestamp       lifetime_;
    
    http::Context   ctx;

    BufferReader reader_;
    BufferWriter writer_;

    void read();
    void write();

};

}
