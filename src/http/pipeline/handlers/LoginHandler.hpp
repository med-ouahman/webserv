#pragma once

#include "pipeline/ARequestHandler.hpp"
#include "Context.hpp"

namespace http {

class LoginHandler: public ARequestHandler {
private:
    std::string extract_value(const std::string& body, const std::string& key);
public:
    LoginHandler(Context&);
    ~LoginHandler();
    Error handle();
};

}