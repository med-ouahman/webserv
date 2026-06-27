#pragma once

#include "Buffer.hpp"
#include <stddef.h>
#include <cstdio>

namespace http {

class IBodyProvider {

public:
    virtual ~IBodyProvider() {};
    virtual ssize_t read(Buffer& dest_, size_t size) = 0;
};

}
