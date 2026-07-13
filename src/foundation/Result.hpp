#pragma once
#include "Error.hpp"
#include <cstring>


namespace base {

template <typename T> struct Result {
bool ok;
T result;
Error error;

Result(const Error& err): ok(false), error(err) {}
Result(const T& res): ok(true), result(res) {}
Result(): ok(true) {}

};

}
