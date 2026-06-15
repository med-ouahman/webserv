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

IBodyProvider::ReadResult FileBodyProvider::read(BufferWriter& writer, size_t size) {
    
    if (fd_ < 0) return Failure;

    ssize_t w = ::read(fd_, writer.write_ptr(), std::min(size, writer.bytes_free()));
    if (w < 0) return Failure;
    return Success;
}


}
}