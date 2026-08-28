
#pragma once

#include "types.hpp"
#include "Optional.hpp"
#include "Expected.hpp"
#include "io/Reader.hpp"
#include "io/Writer.hpp"
#include <sstream>

namespace base {

std::string toLowerCase(const std::string& s);
bool isHex(char c);
usize hexValue(char c);

template <typename T>
std::string to_string(const T& val) {
    std::stringstream ss;
    ss << val;
    return ss.str();
}

}
