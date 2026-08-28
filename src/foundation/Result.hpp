#pragma once
#include "Error.hpp"
#include <cstring>


namespace base {

template <typename T>
class Result {
private:
bool ok_;
T val;
Error error_;

public:
Result(const Error& err): ok_(false), error_(err) {}
Result(const T& res): ok_(true), val(res) {}

bool ok() { return ok_; }

T& value() { return val; }

const Error& error() const { return error_; }

};

}
