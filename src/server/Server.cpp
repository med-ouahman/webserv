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
    conf(c),
    services_(event_loop, logger_, sessions_) {

    logger_.setstream(std::cout);

    logger_.log(logger::Info, "Errors logs are saved to ./var/log/errors.log", true);
    
    running_ = event_loop.created() && start_listeners();
    init_sessions();
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

    for (size_t i = 0; i < sessions_.size(); ++i) {
        delete sessions_[i];
    }
    
    sessions_.clear();
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
	        
            if (l) {
                logger_.log(logger::Info, "Duplicate listen endpoint detected, using existing one", true);
            }

            if (!l) {
                
                if (listeners.size() >= MaxListens) {
                    continue;
                }

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
                
                if (listeners.size() >= MaxListens) {
                    logger_.log(logger::Warning, "Listening sockets maximum reached\nThe next listening sockets will be ignored", true);
                }

                l = sock;
            }
	    
            l->add_server(&servers[i]);
        }
    }

    if (listeners.empty()) {
        logger_.log(logger::Error, "No listening endpoints configured", true);
        return false;
    }
    
    return true;
}

void Server::add_connection(UniqueFd& conn_fd, const std::vector<const config::ServerConfig*>& servers) {

    net::Connection* connection = new (std::nothrow) net::Connection(conn_fd,
        io::Readable,
        servers,
        services_);
    
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
    
    session_cleanup();

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

void Server::init_sessions() {

    for ( size_t i = 0; i < conf.servers.size(); ++i ) {
        
        const config::ServerConfig& server = conf.servers[i];
     
        if (server.session_enabled) {
            http::SessionManager* session = new (std::nothrow) http::SessionManager(server.session_cookie_name,
                server.session_timeout,
                server.session_store);
            if (session) sessions_.push_back(session);
        }
    }
}

void Server::session_cleanup() {
	for ( size_t i = 0; i < sessions_.size(); ++i ) {
		sessions_[i]->cleanup();
	}
}

http::SessionManager* Server::find_session(std::vector<http::SessionManager*>& sessions,
    const std::string& cookie_name) {

    for ( size_t i = 0; i < sessions.size(); i++ ) {
        
        if (sessions[i] != NULL
            && sessions[i]->get_cookie_name() == cookie_name) {
            return sessions[i];
        }
    }
    return NULL;
}

int Server::start() {

    if (!running_) return EXIT_FAILURE;

    while (running_) {
        event_loop.poll();
        maintenance();
    }
    
    return 0;
}
