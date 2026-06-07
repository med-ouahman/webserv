#pragma once

#include "IBodyProvider.hpp"
#include "BufferReader.hpp"

namespace http {

class CGIBodyProvider: public IBodyProvider {
private:
    BufferReader& source;

public:
    CGIBodyProvider(BufferReader& src);
    ssize_t produce(BufferWriter& w);
};

}
