#pragma once

#include "RuntimeServices.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

class Server {

public:
enum ServerErrors {
AllocFailed,
SockFailed,
ConfError,
IOError,
ConnError,

};

static const std::size_t MaxConnections = 1000;
static const std::size_t MaxListens = 10;

static logger::Logger      logger;
private:
    bool running_;
    std::vector<net::Connection*> connections;
    std::vector<net::Socket*> listeners;
    runtime::epoll::EventLoop poller;
    RuntimeServices services_;
    const config::Config& conf;

    Server(const Server&);
    Server& operator=(const Server&);
    bool start_listeners();

public:
    Server(const config::Config& conf);
    ~Server();
    int start();
    void maintenance();
    void abort();
    void add_connection(UniqueFd& uniq, const net::ConnectionInfo& info);
    void close_connection(net::Connection* conn);
    void close_socket(net::Socket* Socket);
    
    net::Socket* find_listener(const config::ListenEndPoint& endpoint);
    size_t num_connections() const;
};
