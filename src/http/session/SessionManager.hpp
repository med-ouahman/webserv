#pragma once

#include <string>
#include <map>
#include <ctime>

namespace http {

class SessionManager {
public:
    static SessionManager& instance();

    void init(const std::string& cookie_name, size_t timeout_seconds);

    std::string create_session();

    bool has_session(const std::string& id);

    void cleanup();

private:
    SessionManager();
    ~SessionManager();

    std::string cookie_name;
    size_t timeout;

    struct SessionData {
        time_t last_touch;
    };

    std::map<std::string, SessionData> sessions;
    
};

}