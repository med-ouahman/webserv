#include "Server.hpp"
#include <cstdlib>
#include <algorithm>
#include "Registrar.hpp"

Server::Server()
    : running_(false),
    poller() {
    running_ = start_listeners();
}

Server::~Server() {

    for (size_t i(0); i < listeners.size(); ++i) {
        delete listeners[i];
    }
    
    listeners.clear();
    
    for (size_t i(0); i < connections.size(); ++i) {
        delete connections[i];
    }
    connections.clear();

}

void Server::server_accept(int conn_fd, void* server_ctx) {
    Server* webserv = static_cast<Server*>(server_ctx);
    webserv->add_connection(conn_fd);
}

void Server::server_register(io::AEventHandler* handler, void* register_ctx) {
    runtime::epoll::EventPoller* loop = static_cast<runtime::epoll::EventPoller*>(register_ctx);
    loop->register_handler(handler);
}

void Server::server_delete(io::AEventHandler* handler, void* register_ctx) {
    runtime::epoll::EventPoller* poller = static_cast<runtime::epoll::EventPoller*>(register_ctx);
    poller->del_handler(handler);
}

void Server::close_connection(net::Connection* conn) {
    connections.erase(
        std::remove(connections.begin(), connections.end(), conn),
        connections.end()
   );

    delete conn;
}

bool Server::start_listeners() {
    
    const std::vector<config::ListenEndPoint>& endpoints = config::Config::get_config().server.listens;
    net::AcceptContext accpet_ctx = {
        .server_ctx = this,
        .callback = server_accept
    };

    for (size_t i(0); i < endpoints.size(); ++i) {

        base::Result<net::Listener*> result = net::create_listening_socket(endpoints[i], accpet_ctx);

        if (!result.ok) return false;
        net::Listener* sock = result.result;
        
        if (!poller.register_handler(sock)) return false;

        listeners.push_back(sock);
    }
    
    return true;
}

void Server::add_connection(int conn_fd) {

    RegisterContext r = {
        .registrar = &poller,
        .callback = server_register,
        .delete_cb = server_delete,
    };

    net::Connection* connection = new net::Connection(conn_fd, io::READABLE, r);
    
    if (!poller.register_handler(&connection->stream()))
        return;

    connections.push_back(connection);
}

/*
    sweep:
    function to be called on each event loop cycle to check the state of the connections
*/

void Server::sweep() {
    for (size_t i(0); i < connections.size();) {
        net::Connection* conn = connections.at(i);
        conn->update_stream();
        poller.sync(&conn->stream());
        if (conn->closing()) close_connection(conn);

        else ++i;
    }

    for (size_t i(0); i < listeners.size(); ++i) {
        net::Listener* listener = listeners.at(i);
        poller.sync(listener);
        if (listener->error()) {
            /*
                What to do here?
                1. close only the listening socket?
                2. shutdown the server?
            */
        }
    }
}

int Server::start() {

    if (!running_)
        return EXIT_FAILURE;
    while (true)
    {
        poller.poll();
        sweep();
    }

}