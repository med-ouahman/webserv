#include "CounterHandler.hpp"
#include <cstdlib>
#include "base.hpp"

namespace http {

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

}