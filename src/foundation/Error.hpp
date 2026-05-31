#pragma once
#include <cerrno>
#include <iostream>

namespace Base
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

