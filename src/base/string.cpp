#include <string>
#include <cctype>
#include "types.hpp"

/*

string utilities

*/

namespace base {

std::string toLowerCase(const std::string& s) {
    std::string result = s;

    for (std::size_t i = 0; i < result.size(); ++i)
    {
        result[i] = static_cast<char>(
            ::tolower(static_cast<unsigned char>(result[i]))
        );
    }

    return result;
}

bool isHex(char c) {
    return (c >= '0' && c <= '9')
        || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F');
}

usize hexValue(char c) {
    if (c >= '0' && c <= '9')
        return static_cast<usize>(c - '0');
    if (c >= 'a' && c <= 'f')
        return static_cast<usize>(c - 'a' + 10);
    return static_cast<usize>(c - 'A' + 10);
}

}
