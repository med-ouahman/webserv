#pragma once

#include "http/pipeline/ARequestHandler.hpp"
#include "Context.hpp"

namespace http {

class CounterHandler: public ARequestHandler {

public:
    explicit CounterHandler(Context&);
    ~CounterHandler();
    Error handle();
};

}