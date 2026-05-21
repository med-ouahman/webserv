#include "Server.hpp"
#include <cstdlib>

Server::Server(): running_(false) {
    
    start_listeners();
    if (!running_) return ;

    
}

Server::~Server() {

}

int Server::start() {
    
    if (!running_) return EXIT_FAILURE;

    return event_loop.run();
}


void Server::server_accept( int conn_fd, net::AcceptContext ctx ) {
    Server* webserv = static_cast<Server*>(ctx);
    webserv->add_connection(conn_fd);
}


void Server::start_listeners() {
    
    const std::vector<config::ListenEndPoint>& endpoints = config::Config::get_config().server.listens;

    for ( size_t i(0); i < endpoints.size(); ++i ) {

        Base::Result<net::ListeningSocket*> result = net::create_listening_socket(endpoints[i], server_accept, this);
        if (!result.ok) {
            running_ = false;
            break;
        }

        listeners.push_back(result.result);
    }

}

void Server::add_connection( int conn_fd ) {

    net::Connection* connection = new net::Connection(conn_fd, io::READABLE);

    if (!event_loop.register_handler(&connection->stream())) return ;
    
    connections.push_back(connection);

}