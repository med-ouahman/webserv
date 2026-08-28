
#include "SessionDemoHandlers.hpp"
#include <cstdlib>

namespace http {

/* Counter */
CounterHandler::CounterHandler(Context& ctx): ARequestHandler(ctx) {}

CounterHandler::~CounterHandler() {}

Error CounterHandler::handle() {
    Request& req = request();

    if (!req.sessionsEnabled) return ERR_UNAUTHORIZED;
    
    SessionManager& session = *req.session;

    const std::string& sid = request().currentSessionID;
	bool valid = request().currentSessionValid;

	if (!valid) {
		const std::string newSessionID = session.create_session();
        session.set_session_data(newSessionID, "counter", "1");
        
        setCookieHeader(session.get_cookie_name()+"="+newSessionID, "Path=/", "HttpOnly; SameSite=Lax");
        
        setContentType("text/html");
        response().body = "<h1>You have been here for 1 time </h1>";
        responseReady();
        return ERR_NONE;
    }

    session.touch_session(sid);

    std::string value = session.get_session_data(sid, "counter");
    
    char* end = NULL;
    size_t counter = std::strtoul(value.c_str(), &end, 10);
    ++counter;
    value = base::to_string(counter);
    session.set_session_data(sid, "counter", value);
    
    response().body = "<h1>You have been here for " + value + " times </h1>";
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
        Notice:
        Login bodies are usually small so it's guarrented that the maximums \
        size of the body will be less than the limit as enforced in config
    */

    Request& req = request();

    if (req.has_body
        && request().body.type() == base::io::Reader::NONE) {
        context_.action_ = AC_READ;
        return ERR_NONE;
    }

    if (!req.sessionsEnabled) return ERR_UNAUTHORIZED;

    std::string body = std::string(req.body.data(), req.body.size());
    SessionManager& session = *req.session;
    
    std::string username = extract_value(body, "username");
    
    if (username.empty()) return ERR_BAD_REQUEST;

    if (req.currentSessionValid) {

        const std::string& sid = req.currentSessionID;

        bool auth = "true" == session.get_session_data(sid, "authenticated")
            && username == session.get_session_data(sid, "username");

        session.touch_session(sid);
        
        if (!auth)
            return ERR_UNAUTHORIZED;
        
        response().body = "<h1>" + session.get_session_data(sid, "username") + " Is already logged in </h1>";
        setContentType("text/html");
        responseReady();
        return ERR_NONE;
    }
        
    std::string newSID = session.create_session();

    session.set_session_data(newSID, "username", username);
    session.set_session_data(newSID, "authenticated", "true");

    setCookieHeader(session.get_cookie_name()+"="+newSID, "Path=/", "HttpOnly; SameSite=Lax");

    response().body = "<h1>Logged in as " + username + "</h1>";
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

    Request& req = request();

    if (!req.sessionsEnabled || !request().currentSessionValid) {
        return ERR_UNAUTHORIZED;
    }

    SessionManager& session = *req.session;

    const std::string& sid = request().currentSessionID;

    bool auth =
        session.get_session_data(sid, "authenticated") == "true";

    if (!auth)
        return ERR_UNAUTHORIZED;

    session.touch_session(sid);

    std::string username = session.get_session_data(sid, "username");

    response().body = "<h1> Welcome " + username + "</h1>";
    setContentType("text/html");
    responseReady();
    return ERR_NONE;
}


/* LOGOUT */

LogoutHandler::LogoutHandler(Context& ctx)
    : ARequestHandler(ctx) {}

LogoutHandler::~LogoutHandler() {}

Error LogoutHandler::handle() {

    Request& req = request();

    if (!req.sessionsEnabled
        || !request().currentSessionValid) return ERR_UNAUTHORIZED;

    SessionManager& session = *req.session;

    const std::string& sid = request().currentSessionID;
    
    bool auth =
        session.get_session_data(sid, "authenticated") == "true";

    if (!auth) return ERR_UNAUTHORIZED;

    session.delete_session(sid);

    setCookieHeader(session.get_cookie_name()+"=", "Path=/", "Max-Age=0; HttpOnly; SameSite=Lax");
    response().body = "<h1>Logged out successfully </h1>";
    setContentType("text/html");
    responseReady();
    return ERR_NONE;
}

}
