#pragma once

#include "BufferWriter.hpp"
#include <stddef.h>
#include <cstdio>

namespace http {


class IBodyProvider {

public:
    enum ReadResult {
        Success,
        Finished,
        Failure,
        Unavailabe,
    };

    virtual ~IBodyProvider() {};
    virtual ReadResult read(BufferWriter& w, size_t size) = 0;
};

}
