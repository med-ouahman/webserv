#pragma once

#include "IBodyProvider.hpp"

namespace http {

class BodyEncoder {

private:
    IBodyProvider* body;

public:
    BodyEncoder();
    ~BodyEncoder();
    ssize_t encode(BufferWriter& writer);
};

}