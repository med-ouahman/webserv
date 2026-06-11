#pragma once

#include <string>

#include "base/base.hpp"
#include "http/Error.hpp"

namespace http {

class Context;

namespace parser {

usize	body_min_size(usize a, usize b);
bool	body_parse_chunk_size(const std::string& line, usize& size);

}
}
