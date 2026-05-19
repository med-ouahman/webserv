#pragma once

#include "Error.hpp"

namespace Error {

class Logger {
    public:
        static void log( const Error& err );
};

}