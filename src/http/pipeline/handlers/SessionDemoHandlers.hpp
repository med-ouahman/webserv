#pragma once


#include "http/pipeline/ARequestHandler.hpp"
#include "Context.hpp"

namespace http {

// COUNTER HANDLER

class CounterHandler: public ARequestHandler {

public:
    explicit CounterHandler(Context&);
    ~CounterHandler();
    Error handle();
};


// LOGIN HANDLER


class LoginHandler: public ARequestHandler {
private:
    std::string extract_value(const std::string& body, const std::string& key);
public:
    LoginHandler(Context&);
    ~LoginHandler();
    Error handle();
};


// PROFILE 
class ProfileHandler: public ARequestHandler {

public:
    ProfileHandler(Context&);
    ~ProfileHandler();
    Error handle();
};


// LOGOUT
class LogoutHandler: public ARequestHandler {

public:
    LogoutHandler(Context&);
    ~LogoutHandler();
    Error handle();
};


}
