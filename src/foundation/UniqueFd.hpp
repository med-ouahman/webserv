#pragma once

#include <fcntl.h>
#include <unistd.h>

class UniqueFd {
private:
    int fd_;

public:
    UniqueFd() : fd_(-1) {}
    explicit UniqueFd(int fd) : fd_(fd) {}

    ~UniqueFd() {
        reset();
    }

    int get() const {
        return fd_;
    }

    void reset() {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

    void set(int fd) {
        reset();
        fd_ = fd;
    }

    int release() {
        int tmp = fd_;
        fd_ = -1;
        return tmp;
    }

    bool valid() const {
        return fd_ >= 0;
    }

private:
    UniqueFd( const UniqueFd& );
    UniqueFd& operator=( const UniqueFd& );
};

