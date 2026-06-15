#pragma once

#include "IBodyProvider.hpp"
#include "BufferReader.hpp"

namespace http {

class CgiHandler;

enum BufferState {
    Filling,
    Draining
};

class CGIBodyProvider: public IBodyProvider {
private:
    CgiHandler& handler;
    BufferReader& source;
    BufferState state_;

public:
    CGIBodyProvider(CgiHandler& handler, BufferReader& src);
    ~CGIBodyProvider();
    ReadResult read(BufferWriter& w, size_t size);
};

}
