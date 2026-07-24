
#pragma once

/*
namespace std {
    
template <typename T>
class basic_string<T>;

typedef basic_string<char> string;


}
*/

#include <string>

namespace server_info {

extern const char* name;
extern const char* version;

std::string info();

}
