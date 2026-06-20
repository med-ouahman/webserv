#include <sys/stat.h>
#include <string>

namespace base {

size_t sizeof_file(std::string const& filename) {
    struct stat buf;

    int r = stat(filename.c_str(), &buf);
    if (r < 0) return 0;

    return buf.st_size;
}

}
