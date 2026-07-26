#pragma once

#include "RuntimeServices.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include "net/socket/Socket.hpp"
#include "Logger.hpp"
#include "ServerErrors.hpp"

class Server {

public:
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
    
    ServerErrors    start();
    void            sweep();
    void            abort();

    void    add_connection(UniqueFd& uniq, const net::ConnectionInfo& info);
    void    close_connection(net::Connection* conn);
    size_t  num_connections() const;
    
    void            close_socket(net::Socket* Socket);
    net::Socket*    find_listener(const config::ListenEndPoint& endpoint);
};
