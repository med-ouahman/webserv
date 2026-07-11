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
 : filename_(filename), fd_(-1), done_(false) {

    fd_ = open(filename_.c_str(), O_RDONLY);

    if (fd_ < 0) {
        /* do nothing for now the read will fail */
        return;
    }

}

FileBodyProvider::FileBodyProvider(int fd)
    : filename_(""), fd_(fd), done_(false) {}

FileBodyProvider::~FileBodyProvider() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

ssize_t FileBodyProvider::read(char* out, size_t size) {
    
    if (done_) return 0;

    ssize_t n = ::read(fd_, out, size);

    if (n == 0) {
        ::close(fd_);
        fd_ = -1;
        done_ = true;
        return 0;
    }

    return n;
}

}
}