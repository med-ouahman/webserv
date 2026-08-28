#pragma once

#include <string>
#include <map>
#include "Request.hpp"

namespace http {

std::string extract_cookie_value(const std::vector<Header>& headers,
    const std::string& cookie_name);

}
