#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>

namespace base {

std::string random_string(std::size_t length) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "_-";

    std::ostringstream oss;
    oss << time(NULL) << '_';
    srand(time(NULL));
    for (std::size_t i = 0; i < length; ++i)
        oss << charset[rand() % (sizeof(charset) - 1)];

    return oss.str();
}

}
