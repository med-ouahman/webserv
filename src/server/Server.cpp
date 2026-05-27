#include "Server.hpp"
#include <cstdlib>
#include <algorithm>

Server::Server()
    : running_(false),
    event_loop(this, server_loop) {
    running_ = start_listeners();
}

Server::~Server() {
    for ( size_t i(0); i < listeners.size(); ++i ) delete listeners[i];
    listeners.clear();
    for ( size_t i(0); i < connections.size(); ++i ) delete connections[i];
    connections.clear();
}

void Server::server_accept( int conn_fd, void* server_ctx ) {
    Server* webserv = static_cast<Server*>(server_ctx);
    webserv->add_connection(conn_fd);
}

void Server::remove_connection( net::Connection* conn ) {
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

    for ( size_t i(0); i < endpoints.size(); ++i ) {

        Base::Result<net::Listener*> result = net::create_listening_socket(endpoints[i], accpet_ctx);

        if (!result.ok) return false;
        net::Listener* sock = result.result;
        if (!event_loop.register_handler(sock)) return false;
        listeners.push_back(sock);
    }
    
    return true;
}

void Server::add_connection( int conn_fd ) {

    net::Connection* connection = new net::Connection(conn_fd, io::READABLE);
    
    if (!event_loop.register_handler(&connection->stream())) return ;

    connections.push_back(connection);
}

int Server::start() {
    if (!running_) return EXIT_FAILURE;
    return event_loop.run();
}


void Server::server_loop( void* server_ctx ) {
    Server* webserv = static_cast<Server*>(server_ctx);
    webserv->sweep();
}

void Server::sweep() {
    for ( size_t i(0); i < connections.size(); ) {
        net::Connection* conn = connections.at(i);

        if (conn->state() == net::CLOSING
            || conn->timedout()) remove_connection(conn);

        else ++i;
    }

    for ( size_t i(0); i < listeners.size(); ++i ) {
        net::Listener* listener = listeners.at(i);
        if (listener->error()) {
            /*
                What to do here?
                1. close only the listening socket
                2. shutdown the server
            */
        }
    }
}