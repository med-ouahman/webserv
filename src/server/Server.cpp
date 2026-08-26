#include "Server.hpp"
#include <cstdlib>
#include <algorithm>
#include "ServerInfo.hpp"
#include "http/session/SessionManager.hpp"

namespace server_info {

const char* name = "VilgaX";
const char* version = "1.0";

std::string info() {
    return name + std::string("/") + version;
}

}

Server::Server(const config::Config& c)
    : running_(false),
    logger_("./var/log/errors.log"),
    event_loop(logger_),
    services_(event_loop, logger_, c),
    conf(c) {

    logger_.setstream(std::cout);
    logger_.log(logger::Info, "Errors logs are saved to ./var/log/errors.log", true);
    running_ = event_loop.created();
    std::stringstream ss;

    running_ = running_ && start_listeners();
    
    http::SessionManager::instance().init("WEBSERVER_SESSSION", 3600);
}

Server::~Server() {

    for (size_t i(0); i < listeners.size(); ++i) {
        event_loop.del(listeners[i]);
        delete listeners[i];
    }
    
    listeners.clear();
    
    for (size_t i(0); i < connections.size(); ++i) {
        event_loop.del(connections[i]);
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
	        if (l) logger_.log(logger::Info, "duplicate ip:port", true);
            if (!l) {

                base::Result<net::Socket*> result = net::create_listening_socket(endpoints[j], *this);
                
                if (!result.ok()) {
                    logger_.log(logger::Error,
                        logger_.make_error(result.error().context,
                        result.error().message,
                        result.error().file,
                        result.error().line), true);

                    return false;
                }
                
                net::Socket* sock = result.value();

                if (!event_loop.add(sock)) return false;
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
        logger_.log(logger::Error, logger_.make_error("Server::add_connection", "allocation failed", __FILE__, __LINE__), true);
        return;
    }

    if (!event_loop.add(connection)) {
        delete connection;
        return;
    }

    connections.push_back(connection);
    logger_.log(logger::Info, "Connection accepted", true);
}


void Server::maintenance() {
    
    /* Session Cleanup */
    http::SessionManager::instance().cleanup();

    for (size_t i(0); i < connections.size();) {
        net::Connection* conn = connections.at(i);

        conn->sync();

        event_loop.sync(conn);

        if (conn->closing()) close_connection(conn);
        
        else ++i;
    }

    for (size_t i(0); i < listeners.size(); ) {
        net::Socket* Socket = listeners.at(i);
        event_loop.sync(Socket);
        if (Socket->error()) {
            close_socket(Socket);
        } else {
            ++i;
        }
    }
}

logger::Logger& Server::logger() { return logger_; }

void Server::abort() {
    std::abort();
}


net::Socket* Server::find_listener(const config::ListenEndPoint& endpoint) {

    for (
        std::vector<net::Socket*>::const_iterator it = listeners.begin();
            it != listeners.end();
            ++it
    ) {
        net::Socket* sock = *it;
        const config::ListenEndPoint& ep = sock->endpoint();
        
        if (net::listeners_match(ep, endpoint)) return sock;
    }

    return NULL;
}

size_t Server::num_connections() const {
    return connections.size();
}

void leaks(bool & r) {

    static Timestamp x;

    if (x.elapsed() >= 400) r = false;

}

int Server::start() {

    if (!running_) return EXIT_FAILURE;

    while (running_) {
        // leaks(running_);
        event_loop.poll();
        maintenance();
    }
    
    return 0;
}
