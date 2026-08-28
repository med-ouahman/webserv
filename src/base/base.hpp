
#pragma once

#include "types.hpp"
#include "Optional.hpp"
#include "Expected.hpp"
#include "io/Reader.hpp"
#include "io/Writer.hpp"
#include "memory/memory.h"
#include <sstream>

namespace base {

size_t sizeof_file(std::string const& filename);

std::string random_string(size_t size);
std::string toLowerCase(const std::string& s);

template <typename T>
std::string to_string(const T& val) {
    std::stringstream ss;
    ss << val;
    return ss.str();
}

}
