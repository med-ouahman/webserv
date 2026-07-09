#pragma once

#include "ServerContext.hpp"
#include "EventPoller.hpp"
#include "Connection.hpp"
#include "Listener.hpp"
#include "Logger.hpp"

class Server {
private:
    bool running_;
    
    static const std::size_t MaxConnections = 1000;
    static const std::size_t MaxListens = 10;

    std::vector<net::Connection*> connections;
    std::vector<net::Listener*> listeners;
    runtime::epoll::EventPoller poller;
    static logger::Logger      logger_;

    ServerContext ctx;

    Server(const Server&);
    Server& operator=(const Server&);
    bool start_listeners();

public:
    Server();
    ~Server();
    int start();
    void sweep();
    void abort();
    void add_connection(int client_fd, const net::ConnectionInfo& info);
    void close_connection(net::Connection* conn);
    static logger::Logger& logger();
    
    net::Listener* find_listener(const config::ListenEndPoint& endpoint);
};
