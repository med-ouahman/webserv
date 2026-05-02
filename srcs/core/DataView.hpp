#pragma once 

#include <stdio.h>

namespace core {

    struct DataView {
        char* data_ptr_;
        size_t len_;
        size_t bytes_consumed;


        DataView(): data_ptr_(NULL), len_(0), bytes_consumed(0) {}
    };
}
