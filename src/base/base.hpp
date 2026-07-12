
#pragma once

#include "types.hpp"
#include "Optional.hpp"
#include "Expected.hpp"
#include "io/Reader.hpp"
#include "io/Writer.hpp"
#include "memory/memory.h"

namespace base {

size_t sizeof_file(std::string const& filename);

std::string random_string(size_t size);

}
