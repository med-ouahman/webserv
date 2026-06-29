#pragma once

#include "IBodyProvider.hpp"
#include "Buffer.hpp"

namespace http {

class CgiHandler;

class CGIBodyProvider: public IBodyProvider {
private:
    CgiHandler& handler;

public:
    CGIBodyProvider(CgiHandler& handler);
    ~CGIBodyProvider();
    ssize_t read(std::string& out, size_t size);
};

}
