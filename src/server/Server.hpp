#pragma once

#include "RuntimeServices.hpp"
#include "EventLoop.hpp"
#include "Connection.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

class Server {

public:

enum ServerErrors {
    AllocFailed,
};

private:
    bool running_;

    std::vector<net::Connection*> connections;
    
    std::vector<net::Socket*> listeners;
    
    logger::Logger  logger_;
    
    runtime::epoll::EventLoop event_loop;
    
    std::vector<http::SessionManager*> sessions_;

    const config::Config& conf;

    RuntimeServices services_;

    Server(const Server&);
    Server& operator=(const Server&);
    bool start_listeners();
    void init_sessions();
    void session_cleanup();

public:
    Server(const config::Config& conf);
    ~Server();
    int start();
    void maintenance();
    void add_connection(UniqueFd& uniq, const std::vector<const config::ServerConfig*>& info);
    void close_connection(net::Connection* conn);
	void close_socket(net::Socket* Socket);

	net::Socket* find_listener(const config::ListenEndPoint& endpoint);
	size_t num_connections() const;
	logger::Logger& logger();

    static http::SessionManager* find_session(std::vector<http::SessionManager*>& sessions,
        const std::string& cookie_name);
};
