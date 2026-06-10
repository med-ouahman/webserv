#pragma once

#include "EventPoller.hpp"
#include "Connection.hpp"
#include "Listener.hpp"

class Server {
private:
    bool running_;
    std::vector<net::Connection*> connections;
    std::vector<net::Listener*> listeners;
    runtime::epoll::EventPoller poller;
    Server(const Server&);
    Server& operator=(const Server&);
    bool start_listeners();
    
public:
    Server();
    ~Server();
    int start();
    void sweep();
    void add_connection(int client_fd);
    void close_connection(net::Connection* conn);
};
