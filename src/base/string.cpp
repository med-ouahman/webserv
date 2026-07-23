#include <string>
#include <cctype>

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

}

