#pragma once

#include <unistd.h>

class Pipe {
private:
    bool created_;
    int read_end_;
    int write_end_;

public:

static bool create(Pipe& out) {
    int fds[2];

    if (::pipe(fds) == -1)
        return false;

    out.read_end_ = fds[0];
    out.write_end_ = fds[1];
    return true;
}

void close() {
    
    if (read_end_ >= 0) {
        ::close(read_end_);
        read_end_ = -1;
    }
    if (write_end_ >= 0) {
        ::close(write_end_);
        write_end_ = -1;
    }
}

int read_end() const {
    return read_end_;
}

int write_end() const {
    return write_end_;
}

void reset() {
    close();
}

Pipe(): created_(create(*this)) {}

void close_write_end() {
    
    if (read_end_ < 0) return;
    ::close(write_end_);
    write_end_ = -1;
}

void close_read_end() {
    if (read_end_ < 0) return;
    ::close(read_end_);
    read_end_ = -1;
}

~Pipe() {
    close();
}

operator bool() {
    return created_;
}

};

