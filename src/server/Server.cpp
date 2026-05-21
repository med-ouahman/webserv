#include "Server.hpp"


Server::Server() {

    Base::Result<std::vector<net::ListeningSocket*> > result
        = net::create_listening_sockets(config::Config::get_config().server.listens, server_accept, this);
    
    if (!result.ok) return 1;

    std::vector<net::ListeningSocket*>& listeners = result.result;
}

Server::~Server() {

}

int Server::start() {


    runtime::epoll::EventLoop event_loop;
    
    event_loop.run();

    return 0;
}


void Server::server_accept( int conn_fd, Server* webserv ) {
    webserv->add_new_connection(conn_fd);
}
