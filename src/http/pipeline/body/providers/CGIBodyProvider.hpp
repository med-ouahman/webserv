#pragma once

#include "IBodyProvider.hpp"
#include "BufferView.hpp"

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
    CGIBodyProvider(CgiHandler& handler, BufferView& src);
    ~CGIBodyProvider();
    ssize_t read(BufferWriter& w, size_t size);
};

}
