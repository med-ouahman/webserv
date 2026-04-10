#pragma once
#include "Error.hpp"

namespace Error {

template <typename T> struct Result {
        bool ok;
        T result;
        Error err;
};

}
