#include "LoginHandler.hpp"

namespace http {

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
        std::cout << "Waiting for body\n";
        context_.action_ = AC_READ;
        return ERR_NONE;
    }

    SessionManager& sessions = SessionManager::instance();

    if (!request().currentSessionValid) {
        std::string newSID = sessions.create_session();
        setHeader("Set-Cookie", sessions.get_cookie_name()+"="+newSID+"; Path=/");
        request().currentSessionID = newSID;
    }

    const std::string& sid = request().currentSessionID;
    std::cout << "Session ID: " << sid << "\n";

    std::string body = std::string(request().body.data(),
        request().body.size());

    std::string username = extract_value(body, "username");
    
    if (username.empty()) {
        return ERR_BAD_REQUEST;
    }

    sessions.set_session_data(sid, "username", username);
    sessions.set_session_data(sid, "authenticated", "true");
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

}