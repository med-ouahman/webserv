#pragma once

#include <unistd.h>

class UniqueFd {
private:
int fd_;
UniqueFd(const UniqueFd& other);
UniqueFd& operator=(const UniqueFd& other);

public:
UniqueFd() : fd_(-1) {}

explicit UniqueFd(int fd) : fd_(fd) {}

~UniqueFd() { reset(); }

int get() const {
    return fd_;
}

bool valid() const {
    return fd_ >= 0;
}

void reset(int newfd = -1) {
    if (fd_ >= 0)
        ::close(fd_);
    fd_ = newfd;
}

int release() {
    int tmp = fd_;
    fd_ = -1;
    return tmp;
}


};
