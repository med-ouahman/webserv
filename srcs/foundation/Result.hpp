#pragma once
#include "Error.hpp"
#include <string.h>

#define MAKE_ERROR(code, context, message) \
        error::Error(code, context, message, __FILE__, __LINE__)

#define MAKE_ERRNO_ERROR(context) \
        error::Error(errno, context, strerror(errno), __FILE__, __LINE__) 
        
#define LOG_ERROR(error) \
        std::cerr << "[ERROR] " << error.context << " | code=" \
        <<  error.code << " | " \
        << error.message << " | " \
        << error.file << ":" << error.line << "\n"

namespace error {

template <typename T> struct Result {
        bool ok;
        T result;
        Error error;

        Result( const Error& err ): ok(false), error(err) {}
        Result( const T& res ): ok(true), result(res) {}
        Result(): ok(true) {}
};

}
