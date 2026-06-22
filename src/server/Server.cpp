#include "Server.hpp"
#include <cstdlib>
#include <algorithm>

logger::Logger Server::logger_;

Server::Server()
    : running_(false),
    poller() {
    ctx.poller = &poller;
    ctx.logger = &logger_;
    logger_.setstream(std::cout);

    running_ = poller.created();

    running_ = running_ && start_listeners();
}

logger::Logger& Server::logger() {
    return logger_;
}

Server::~Server() {

    for (size_t i(0); i < listeners.size(); ++i) {
        poller.del(listeners[i]);
        delete listeners[i];
    }
    
    listeners.clear();
    
    for (size_t i(0); i < connections.size(); ++i) {
        poller.del(connections[i]);
        delete connections[i];
    }

    connections.clear();
}

void Server::close_connection(net::Connection* conn) {
    connections.erase(
        std::remove(connections.begin(), connections.end(), conn),
        connections.end()
    );

    std::cout << "DELETED CONNECTION: " << conn << "FD: " << conn->fd() << "\n";
    
    delete conn;
}

bool Server::start_listeners() {
    
    const std::vector<config::ListenEndPoint>& endpoints = config::Config::get_config().server.listens;
  
    for (size_t i(0); i < endpoints.size(); ++i) {

        base::Result<net::Listener*> result = net::create_listening_socket(endpoints[i], *this);

        if (!result.ok) {
            LOG_ERROR(result.error);
            return false;
        }

        net::Listener* sock = result.result;
        
        if (!poller.add(sock)) return false;

        listeners.push_back(sock);
    }
    
    return true;
}

void Server::add_connection(int conn_fd, const net::ConnectionInfo& info) {

    net::Connection* connection = new net::Connection(conn_fd, io::Readable, ctx, info);
    
    if (!poller.add(connection)) return;

    connections.push_back(connection);
}

/*
    sweep:
    function to be called on each event loop cycle to check the state of the connections
*/

void Server::sweep() {

    for (size_t i(0); i < connections.size();) {
        net::Connection* conn = connections.at(i);

        conn->sync();

        poller.sync(conn);

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

    if (!running_) return EXIT_FAILURE;
    
    while (running_) {
        poller.poll();
        sweep();
    }
    
    return 0;
}

void Server::abort() {
    std::abort();
}