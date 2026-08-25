#include "CookieHandlers.hpp"

namespace http {

/* Counter */
CounterHandler::CounterHandler(Context& ctx): ARequestHandler(ctx) {


}

CounterHandler::~CounterHandler() {

}

Error CounterHandler::handle() {
    SessionManager& sessions = SessionManager::instance();
    
    const std::string& sid = request().currentSessionID;
    bool valid = request().currentSessionValid;

    if (!valid) {
        std::cout << "Cookie name: " << sessions.get_cookie_name() << "\n";
        const std::string newSessionID = sessions.create_session();
        sessions.set_session_data(newSessionID, "counter", "1");
        std::string cookieHeaderValue = sessions.get_cookie_name()+"="+newSessionID+"; Path=/";
        setHeader("Set-Cookie", cookieHeaderValue);
        setContentType("text/html");
        response().body = "You have been here for 1 time";
        responseReady();
        return ERR_NONE;
    }

    sessions.touch_session(sid);

    std::string value = sessions.get_session_data(sid, "counter");
    
    char* end = NULL;
    size_t counter = std::strtoul(value.c_str(), &end, 10);
    ++counter;
    value = base::to_string(counter);
    sessions.set_session_data(sid, "counter", value);
    
    response().body = "You have been here for: " + value + " times";
    setContentType("text/html");
    responseReady();
    return ERR_NONE;
}

/* Login */

LoginHandler::LoginHandler(Context& ctx): ARequestHandler(ctx) {

}

LoginHandler::~LoginHandler() {

}

Error LoginHandler::handle() {

    /*
        Notice
        Login bodies are usually small so it's guarrented that the maximums \
        size of the body will be less than the limit as enforced in config
    */

    if (request().has_body
        && request().body.type() == base::io::Reader::NONE) {
        context_.action_ = AC_READ;
        return ERR_NONE;
    }

    SessionManager& sessions = SessionManager::instance();

    if (request().currentSessionValid) {

        const std::string& sid = request().currentSessionID;

        bool auth = "true" == sessions.get_session_data(sid, "authenticated");

        sessions.touch_session(sid);
        
        if (!auth)
            return ERR_UNAUTHORIZED;
        
        response().body = sessions.get_session_data("sid", "username") + " Is already logged in";
        setContentType("text/html");
        responseReady();
        return ERR_NONE;
    }

     std::string newSID = sessions.create_session();
        setHeader("Set-Cookie", sessions.get_cookie_name()+"="+newSID+"; Path=/");
        
    std::string body = std::string(request().body.data(),
        request().body.size());

    std::string username = extract_value(body, "username");
    
    if (username.empty()) return ERR_BAD_REQUEST;

    sessions.set_session_data(newSID, "username", username);
    sessions.set_session_data(newSID, "authenticated", "true");

    response().body = "Logged in as " + username;
    setContentType("text/html");

    responseReady();
    return ERR_NONE;
}

std::string LoginHandler::extract_value(
    const std::string& body,
    const std::string& key)
{
    size_t key_start = body.find(key);

    if (key_start == std::string::npos)
        return "";

    if (key_start != 0 && body[key_start - 1] != '&')
        return "";

    size_t equal = key_start + key.size();

    if (equal >= body.size() || body[equal] != '=')
        return "";

    size_t value_start = equal + 1;

    size_t value_end = body.find('&', value_start);

    if (value_end == std::string::npos)
        value_end = body.size();

    return body.substr(value_start, value_end - value_start);
}

/* Profile */

ProfileHandler::ProfileHandler(Context& ctx)
    : ARequestHandler(ctx) {}

ProfileHandler::~ProfileHandler() {}

Error ProfileHandler::handle() {

    SessionManager& sessions = SessionManager::instance();

    if (!request().currentSessionValid)
        return ERR_UNAUTHORIZED;

    const std::string& sid = request().currentSessionID;

    bool auth =
        sessions.get_session_data(sid, "authenticated") == "true";

    if (!auth)
        return ERR_UNAUTHORIZED;

    sessions.touch_session(sid);

    std::string username =
        sessions.get_session_data(sid, "username");

    response().body = "Welcome " + username;
    setContentType("text/html");
    responseReady();
    return ERR_NONE;
}


/* LOGOUT */

LogoutHandler::LogoutHandler(Context& ctx)
    : ARequestHandler(ctx) {}

LogoutHandler::~LogoutHandler() {}

Error LogoutHandler::handle() {
    SessionManager& sessions = SessionManager::instance();

    if (!request().currentSessionValid)
        return ERR_UNAUTHORIZED;

    const std::string& sid = request().currentSessionID;
    bool auth =
        sessions.get_session_data(sid, "authenticated") == "true";

    if (!auth)
        return ERR_UNAUTHORIZED;
        
    sessions.delete_session(sid);
}

}
