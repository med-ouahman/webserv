#include "Server.hpp"
#include <cstdlib>
#include <algorithm>

Server::Server(): running_(false) {
    running_ = start_listeners();
}

Server::~Server() {}

int Server::start() {
    if (!running_) return EXIT_FAILURE;
    return event_loop.run();
}


void Server::server_accept( int conn_fd, net::AcceptContext ctx ) {
    Server* webserv = static_cast<Server*>(ctx);
    webserv->add_connection(conn_fd);
}

void Server::server_disconnect( net::ServerContext ctx, net::Connection* conn ) {
    Server* webserv = static_cast<Server*>(ctx.context);

    webserv->remove_connection(conn);
}

void Server::remove_connection(net::Connection* conn) {
    connections.erase(
        std::remove(connections.begin(), connections.end(), conn),
        connections.end()
    );

    delete conn;
}

bool Server::start_listeners() {
    
    const std::vector<config::ListenEndPoint>& endpoints = config::Config::get_config().server.listens;

    for ( size_t i(0); i < endpoints.size(); ++i ) {

        Base::Result<net::ListeningSocket*> result = net::create_listening_socket(endpoints[i],
            server_accept,
            this);

        if (!result.ok) return false;
        net::ListeningSocket* sock = result.result;
        if (!event_loop.register_handler(sock)) return false;
        listeners.push_back(sock);
    }
    
    return true;
}

void Server::add_connection( int conn_fd ) {

    net::ServerContext ctx = { .context = this };
    net::Connection* connection = new net::Connection(conn_fd, io::READABLE, ctx, server_disconnect);
    
    if (!event_loop.register_handler(&connection->stream())) return ;

    connections.push_back(connection);

}