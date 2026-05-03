#pragma once

#include <string>
#include <map>
#include <ctime>

namespace core {

class SessionManager {
public:
    static SessionManager& instance();

    void init(const std::string& cookie_name, size_t timeout_seconds);

    // create a new session id and an empty session map
    std::string create_session();

    // check existence
    bool has_session(const std::string& id);

    // simple cleanup of expired sessions (not aggressive)
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
