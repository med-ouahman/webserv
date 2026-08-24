#include "SessionManager.hpp"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

namespace http {

SessionManager& SessionManager::instance() {
    static SessionManager inst;
    return inst;
}

SessionManager::SessionManager(): cookie_name("SESSIONID"), timeout(3600), sessions() {
    std::srand((unsigned)std::time(0));
}

SessionManager::~SessionManager() {}

void SessionManager::init(const std::string& cookie_name_, size_t timeout_seconds) {
    cookie_name = cookie_name_.empty() ? "SESSIONID" : cookie_name_;
    timeout = timeout_seconds == 0 ? 3600 : timeout_seconds;
}

bool SessionManager::is_initialized() const {

}

static std::string gen_random_hex(size_t len) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        int r = std::rand() & 0xff;
        ss << std::setw(2) << (r & 0xff);
    }
    return ss.str();
}

std::string SessionManager::create_session() {
    std::string id = gen_random_hex(16);
    SessionData d;
    d.last_touch = std::time(0);
    sessions[id] = d;
    return id;
}

bool SessionManager::has_session_data(const std::string& id, const std::string& key) const {
    std::map<std::string, SessionData>::const_iterator it = sessions.find(id);
    
    if (it == sessions.end()) return false;
    
    return true;
}

void SessionManager::cleanup() {
    time_t now = std::time(0);
    for (std::map<std::string, SessionData>::iterator it = sessions.begin(); it != sessions.end();) {
        if ((size_t)(now - it->second.last_touch) > timeout) {
            std::map<std::string, SessionData>::iterator to_erase = it;
            ++it;
            sessions.erase(to_erase->first);
        } else {
            ++it;
        }
    }
}

std::string SessionManager::generate_session_id() const {

}

void SessionManager::set_session_data(const std::string& id,
    const std::string& key,
    const std::string& value) {

}

std::string SessionManager::get_session_data(const std::string& id,
    const std::string& key) const {

}

bool SessionManager::has_session(const std::string& id) const {

}
void SessionManager::touch_session(const std::string& id) {

}

void SessionManager::delete_session(const std::string& id) {

}

}
