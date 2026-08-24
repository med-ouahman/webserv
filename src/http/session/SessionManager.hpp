#pragma once

#include <map>
#include <string>

namespace http {

class SessionManager {
public:
    static SessionManager& instance();
    
    // Initialization
    void init(const std::string& cookie_name, size_t timeout_seconds);
    bool is_initialized() const;
    
    // Session lifecycle
    std::string create_session();           // Create new session
    bool has_session(const std::string& id) const;
    void touch_session(const std::string& id);  // Refresh timeout
    void delete_session(const std::string& id); // Explicit invalidation (logout)
    
    // Session data storage
    void set_session_data(const std::string& id, 
        const std::string& key, 
        const std::string& value);
    std::string get_session_data(const std::string& id, 
        const std::string& key) const;
    bool has_session_data(const std::string& id, 
        const std::string& key) const;
    
    // Maintenance
    void cleanup();
    size_t get_session_count() const;
    
    // Config access
    std::string get_cookie_name() const;
    
private:
    SessionManager();
    ~SessionManager();
    SessionManager(const SessionManager&);
    SessionManager& operator=(const SessionManager&);
    
    std::string generate_session_id() const;
    
    std::string cookie_name;
    size_t timeout;
    bool initialized;
    
    struct SessionData {
        time_t creation_time;
        time_t last_touch;
        std::map<std::string, std::string> data;  // Application data
    };
    
    std::map<std::string, SessionData> sessions;
};

}  // namespace http
