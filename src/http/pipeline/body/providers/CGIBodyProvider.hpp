#pragma once

#include "IBodyProvider.hpp"
#include "BufferReader.hpp"

namespace http {

class CGIBodyProvider: public IBodyProvider {
private:
    BufferReader& source;

public:
    CGIBodyProvider(BufferReader& src);
    ~CGIBodyProvider();
    ssize_t read(BufferWriter& w, size_t size);
};

}
