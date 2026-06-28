#include "FileBodyProvider.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <iostream>
#include <cerrno>

namespace http {
namespace body {

FileBodyProvider::FileBodyProvider(const std::string& filename)
 : filename_(filename), fd_(-1) {

    fd_ = open(filename_.c_str(), O_RDONLY);

    if (fd_ < 0) {
        /* do nothing for now the read will fail */
        return;
    }

}

FileBodyProvider::FileBodyProvider(int fd)
    : filename_(""), fd_(fd) {}

FileBodyProvider::~FileBodyProvider() {
    if (fd_ >= 0) ::close(fd_);
    fd_ = -1;
}

ssize_t FileBodyProvider::read(std::string& out, size_t size) {
    
    ssize_t n = ::read(fd_, &out[0], size);

    return n;
}

}
}