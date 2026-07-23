#include "Server.hpp"
#include <cstdlib>
#include <algorithm>


static void close_server_for_memory_checks() {
    static Timestamp t(0);

    if (t.seconds() == 0) {
        t.update();
    }

    if (t.elapsed()>= 30) abort();

    return;
    close_server_for_memory_checks();
}

logger::Logger Server::logger;

Server::Server(const config::Config& c)
    : running_(false),
    poller(),
    services_(poller, logger, c),
    conf(c) {

    logger.setstream(std::cout);
    running_ = poller.created();
    running_ = running_ && start_listeners();
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
    
    delete conn;
}

void Server::close_socket(net::Socket* Socket) {
    listeners.erase(
        std::remove(listeners.begin(), listeners.end(), Socket),
        listeners.end()
    );
    
    delete Socket;
}

bool Server::start_listeners() {
    
    const std::vector<config::ServerConfig>& servers = conf.servers;

    for (size_t i(0); i < servers.size(); ++i) {

        const std::vector<config::ListenEndPoint>& endpoints = servers[i].listens;

        for (size_t j(0); j < endpoints.size(); ++j) {
            
            net::Socket* l = find_listener(endpoints[j]);

            if (!l) {

                base::Result<net::Socket*> result = net::create_listening_socket(endpoints[j], *this);
                
                if (!result.ok()) {
                    logger.log(logger::Error,
                        logger.make_error(result.error().context,
                        result.error().message,
                        result.error().file,
                        result.error().line), true);

                    return false;
                }
                
                net::Socket* sock = result.value();

                if (!poller.add(sock)) return false;
                listeners.push_back(sock);

                l = sock;
            }

            l->add_server(&servers[i]);
        }
    }
    
    return true;
}

void Server::add_connection(UniqueFd& conn_fd, const net::ConnectionInfo& info) {

    net::Connection* connection = new (std::nothrow) net::Connection(conn_fd, io::Readable, services_, info);
    
    if (!connection) {
        logger.log(logger::Error, logger.make_error("Server::add_connection", "allocation failed", __FILE__, __LINE__), true);
        return;
    }

    if (!poller.add(connection)) {
        delete connection;
        return;
    }

    connections.push_back(connection);
    logger.log(logger::Info, "Connection accepted", true);
}


void Server::sweep() {
    
    for (size_t i(0); i < connections.size();) {
        net::Connection* conn = connections.at(i);

        conn->sync();

        poller.sync(conn);

        if (conn->closing()) close_connection(conn);
        
        else ++i;
    }

    for (size_t i(0); i < listeners.size(); ) {
        net::Socket* Socket = listeners.at(i);
        poller.sync(Socket);
        if (Socket->error()) {

            close_socket(Socket);
        } else {
            ++i;
        }
    }
}

void Server::abort() {
    std::abort();
}

net::Socket* Server::find_listener(const config::ListenEndPoint& endpoint) {

    for (
        std::vector<net::Socket*>::const_iterator it = listeners.begin();
            it != listeners.end();
            ++it
    ) {
        net::Socket* Socket = *it;
        const config::ListenEndPoint& e = Socket->endpoint();
        
        if (endpoint.host == e.host && endpoint.port == e.port) return Socket;
    }

    return NULL;
}

size_t Server::num_connections() const {
    return connections.size();
}

int Server::start() {

    if (!running_) return EXIT_FAILURE;

    while (running_) {
        poller.poll();
        sweep();
    }
    
    return 0;
}
