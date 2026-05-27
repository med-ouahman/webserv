#pragma once

#include "Request.hpp"
#include "LineScanner.hpp"
#include "Expected.hpp"

namespace http {
namespace parser {

bool validate_version( std::string const& version_str );
void normalize_header_name( std::string& name );
std::string capitalize_header_name( const std::string& name );
bool validate_header_name( const std::string& name );
Base::Expected<std::pair<std::string, std::string>, int> parse_header( const std::string& line );

}
}
