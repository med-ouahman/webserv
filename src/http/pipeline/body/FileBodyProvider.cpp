#include "FileBodyProvider.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

namespace http {
namespace body {

FileBodyProvider::FileBodyProvider(const std::string& filename)
 : filename_(filename), fd_(-1), size(0) {

    fd_ = open(filename_.c_str(), O_RDONLY);
    if (fd_ < 0) {
        /* do nothing for now the read will fail */
        return;
    }

    size = file_size(filename_); 
}

FileBodyProvider::~FileBodyProvider() {
    ::close(fd_);
    fd_ = -1;
}

size_t FileBodyProvider::file_size(std::string const& filename) {
    struct stat buff;

    if (0 > stat(filename.c_str(), &buff)) return 0;

    return buff.st_size;
}

ssize_t FileBodyProvider::read(BufferWriter& writer) {
    
    if (fd_ < 0) return -1;

    return ::read(fd_, writer.write_ptr(), writer.bytes_free());
}


}
}