#pragma once
#include <cerrno>
#include <iostream>

namespace base
{

struct Error {
    int code;
    const char* context;
    const char* message;
    const char* file;
    int line;

    Error(): code(0), context(""), message(""), file(""), line(0) {}
    Error(int code_, const char* ctx, const char* msg, const char* fl, int l)
    : code(code_),
    context(ctx),
    message(msg),
    file(fl),
    line(l)
    {}
};

}

#define MAKE_ERROR(code, context, message) \
        base::Error(code, context, message, __FILE__, __LINE__)

#define MAKE_ERRNO_ERROR(context) \
        base::Error(errno, context, strerror(errno), __FILE__, __LINE__) 
        
#define LOG_ERROR(error) \
        std::cerr << "[ERROR] " << error.context << " | code=" \
        <<  error.code << " | " \
        << error.message << " | " \
        << error.file << ":" << error.line << "\n"
