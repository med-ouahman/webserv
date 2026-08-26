#pragma once

#include "AEventHandler.hpp"
#include "Timestamp.hpp"
#include "Context.hpp"
#include "BufferView.hpp"
#include "Buffer.hpp"
#include "UniqueFd.hpp"

struct RuntimeServices;

namespace net {

enum ConnectionState {
    Reading,
    Writing,
    Closing,
};

class Connection: public io::AEventHandler {  
public:
    const static std::size_t ReadbufSize    = 1024 * 16;
    const static std::size_t WritebufSize   = 1024 * 16;

    Connection(UniqueFd& fd, io::Event events,
        const std::vector<const config::ServerConfig*>& servers,
        RuntimeServices& services);
    ~Connection();
    void on_event(io::Event events);
    bool closing() const;
    void sync();

private:
    ConnectionState state_;
    
    http::Context   ctx;

    Storage<ReadbufSize> in;
    Storage<WritebufSize> out;

    Buffer reader_;
    Buffer writer_;

    void read();
    void write();
    void on_writable();
    void on_readable();
    void update(http::ContextAction action);
    
};

}
