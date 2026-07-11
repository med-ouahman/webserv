#pragma once

#include "BufferWriter.hpp"
#include <stddef.h>
#include <cstdio>

namespace http {

class IBodyProvider {

public:
    virtual ~IBodyProvider() {};
    virtual ssize_t read(BufferWriter& w, size_t size) = 0;
};

}
