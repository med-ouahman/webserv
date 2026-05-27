#pragma once

#include "EventLoop.hpp"
#include "Connection.hpp"
#include "Listener.hpp"

class Server {
private:
    bool running_;
    std::vector<net::Connection*> connections;
    std::vector<net::Listener*> listeners;
    runtime::epoll::EventLoop event_loop;

    Server(const Server& );
    Server& operator=( const Server& );
    bool start_listeners();
    void add_connection( int client_fd );
    void remove_connection( net::Connection* conn );
public:
    Server();
    ~Server();
    int start();
    void sweep();
    static void server_accept( int conn_fd, void* server_ctx );
    static void server_loop( void* server_ctx );
};
