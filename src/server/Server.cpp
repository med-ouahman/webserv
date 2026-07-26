#include "Server.hpp"
#include <cstdlib>
#include <algorithm>
#include "ServerInfo.hpp"
#include <sstream>

namespace server_info {

const char* name = "VilgaX";
const char* version = "1.0";

std::string info() {
    return name + std::string("/") + version;
}

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

void Server::close_socket(net::Socket* sock) {
    listeners.erase(
        std::remove(listeners.begin(), listeners.end(), sock),
        listeners.end()
    );
    
    delete sock;
}

bool Server::start_listeners() {
    
    const std::vector<config::ServerConfig>& servers = conf.servers;

    for (size_t i(0); i < servers.size(); ++i) {

        const std::vector<config::ListenEndPoint>& endpoints = servers[i].listens;

        for ( size_t j(0); j < endpoints.size(); ++j ) {
            
            net::Socket* l = find_listener(endpoints[j]);

            if (l) {
                std::stringstream ss;
                ss << "Duplicate listen endpoints on "
                << net::int_to_ip(l->endpoint().host) << ":" << l->endpoint().port;
                logger.log(logger::Info, ss.str());
            }

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
        logger.log(logger::Error,
            logger.make_error("Server::add_connection",
                "allocation failed",
                __FILE__,
                __LINE__),
                true);
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
    
    for ( size_t i(0); i < connections.size(); ) {
        
        net::Connection* conn = connections.at(i);

        conn->sync();

        poller.sync(conn);

        if (conn->closing()) close_connection(conn);
        
        else ++i;
    }

    for ( size_t i(0); i < listeners.size(); ) {
        net::Socket* sock = listeners.at(i);

        poller.sync(sock);

        if (sock->error()) close_socket(sock);
        else ++i;
    }
}

void Server::abort() {
    running_ = false;
}

net::Socket* Server::find_listener(const config::ListenEndPoint& endpoint) {

    for (
        std::vector<net::Socket*>::const_iterator it = listeners.begin();
            it != listeners.end();
            ++it
    ) {
        net::Socket* sock = *it;
        const config::ListenEndPoint& e = sock->endpoint();
        
        if (endpoint.host == e.host && endpoint.port == e.port) return sock;
    }

    return NULL;
}

size_t Server::num_connections() const {
    return connections.size();
}

void leaks(bool & r) {

    static Timestamp x;

    if (x.elapsed() >= 100) r = false;


}

ServerErrors Server::start() {

    if (!running_) return IOError;

    while (running_) {
        // leaks(running_);
        poller.poll();
        sweep();
    }
    
    return None;
}
