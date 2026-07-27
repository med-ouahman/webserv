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

struct ConnectionInfo {
uint32_t local_ip;
uint16_t local_port;

uint32_t remote_ip;
uint16_t remote_port;

std::vector<const config::ServerConfig*>& servers;

ConnectionInfo(uint32_t local_ip,
uint16_t local_port,
uint32_t remote_ip,
uint16_t remote_port,
std::vector<const config::ServerConfig*>& srvs
)

: local_ip(local_ip),
local_port(local_port),
remote_ip(remote_ip),
remote_port(remote_port),
servers(srvs) {}
};


class Connection: public io::AEventHandler {  
public:
    const static std::size_t ReadbufSize    = 1024 * 1024;
    const static std::size_t WritebufSize   = 1024 * 1024;

    Connection(UniqueFd& fd, io::Event events, RuntimeServices& ctx, const ConnectionInfo& info);
    ~Connection();
    void on_event(io::Event events);
    bool closing() const;
    void sync();

   

private:
    ConnectionState state_;
    bool            close_after_write;

    Timestamp       last_activity_;
    Timestamp       lifetime_;
    
    http::Context   ctx;

    Storage<ReadbufSize> in;
    Storage<WritebufSize> out;

    Buffer reader_;
    Buffer writer_;

    const ConnectionInfo info_;

    void read();
    void write();
    void on_writable();
    void on_readable();
    void update(http::ContextAction action);
    
};

}
