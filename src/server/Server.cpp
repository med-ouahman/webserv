#include "Server.hpp"
#include <cstdlib>
#include <algorithm>

Server::Server()
    : running_(false),
    event_loop() {
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




void Server::remove_connection(net::Connection* conn) {
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

        Base::Result<net::Listener*> result = net::create_listening_socket(endpoints[i], accpet_ctx);

        if (!result.ok) return false;
        net::Listener* sock = result.result;
        if (!event_loop.register_handler(sock)) return false;
        listeners.push_back(sock);
    }
    
    return true;
}




void Server::add_connection(int conn_fd) {

    net::Connection* connection = new net::Connection(conn_fd, io::READABLE);
    
    if (!event_loop.register_handler(&connection->stream()))
        return;

    connections.push_back(connection);
}




/*
    server_loop:
    acts like a brigde between the server and the event loop, it makes the event loop unawere of the type of the server.
*/
void Server::server_loop(void* server_ctx) {
    Server* webserv = static_cast<Server*>(server_ctx);
    webserv->sweep();
}





/*
    sweep:
    function to be called on each event loop cycle to check the state of the connections
*/
void Server::sweep() {
    for (size_t i(0); i < connections.size();) {
        net::Connection* conn = connections.at(i);

        if (conn->state() == net::CLOSING
            || conn->timedout()) remove_connection(conn);

        else ++i;
    }

    for (size_t i(0); i < listeners.size(); ++i) {
        net::Listener* listener = listeners.at(i);
        if (listener->error()) {
            /*
                What to do here?
                1. close only the listening socket?
                2. shutdown the server?
            */
        }
    }
}

/*
    Design question:
    Currently the EventLoop is the one that keeps the server alive, but that doesn't make sense,
     because all the event loop is to watch and return ready events.
    It should know if the server will be running forever or just for one
    the while (true) loop should be in the server and the event loop is called on each cycle.
*/

int Server::start() {

    if (!running_)
        return EXIT_FAILURE;
    while (true)
    {
        event_loop.loop();
        sweep();
    }

}