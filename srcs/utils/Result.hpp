#pragma once
#include "Error.hpp"

namespace error {

template <typename T> struct Result {
        bool ok;
        T result;
        Error err;
};

}
