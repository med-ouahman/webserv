#pragma once

#include "Buffer.hpp"
#include <stddef.h>
#include <cstdio>

namespace http {

class IBodyProvider {
public:
    virtual ~IBodyProvider() {};
    virtual ssize_t read(std::string& out, size_t max_size) = 0;
};

}
