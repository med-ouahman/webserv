#pragma once

#include "UniqueFd.hpp"
#include <unistd.h>


class Pipe {
private:
    bool created_;
    UniqueFd read_end_;
    UniqueFd write_end_;

public:
static bool create( Pipe& out ) {
    int fds[2];

    if (::pipe(fds) == -1)
        return false;

    out.read_end_.set(fds[0]);
    out.write_end_.set(fds[1]);
    return true;
}

void close() {
    read_end_.close();
    write_end_.close();
}

UniqueFd& read_end() {
    return read_end_;
}

UniqueFd& write_end() {
    return write_end_;
}

void reset() {
    read_end_.close();
    write_end_.close();
}

Pipe(): created_(create(*this)) {}


~Pipe() {
    read_end_.close();
    write_end_.close();
}

operator bool() {
    return created_;
}
};

