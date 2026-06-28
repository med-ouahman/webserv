#pragma once

#include "IBodyProvider.hpp"
#include "Buffer.hpp"

namespace http {

class CgiHandler;

enum BufferState {
    Filling,
    Draining
};

class CGIBodyProvider: public IBodyProvider {
private:
    CgiHandler& handler;
    BufferView& source;
    BufferState state_;

public:
    CGIBodyProvider(CgiHandler& handler, BuffeView& src);
    ~CGIBodyProvider();
    ssize_t read(std::string& out, size_t size);
};

}
