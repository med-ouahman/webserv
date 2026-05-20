#include "Server.hpp"


Server::Server() {

}

Server::~Server() {

}

int Server::start() {

    Base::Result<std::vector<net::ListeningSocket> > result
        = net::create_listening_sockets(config::Config::get_config().server.listens);
    
    if (!result.ok) return 1;

    std::vector<net::ListeningSocket>& listeners = result.result;

    runtime::epoll::EventLoop event_loop;
    
    event_loop.run();

    return 0;
}


