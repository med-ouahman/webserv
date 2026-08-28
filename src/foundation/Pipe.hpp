#pragma once

#include <fcntl.h>
#include <unistd.h>

class Pipe {
private:
    bool created_;
    int read_end_;
    int write_end_;

public:

static bool create(Pipe& out) {
    int fds[2];

    if (::pipe(fds) == -1) {
        return false;
	}
	int read_flags = ::fcntl(fds[0], F_GETFD);
	int write_flags = ::fcntl(fds[1], F_GETFD);
	if (read_flags < 0 || write_flags < 0
		|| ::fcntl(fds[0], F_SETFD, read_flags | FD_CLOEXEC) < 0
		|| ::fcntl(fds[1], F_SETFD, write_flags | FD_CLOEXEC) < 0) {
		::close(fds[0]);
		::close(fds[1]);
		return false;
	}

    out.read_end_ = fds[0];
    out.write_end_ = fds[1];
    return true;
}

void close() {
    
   close_write_end();
   close_read_end();
}

int read_end() const {
    return read_end_;
}

int write_end() const {
    return write_end_;
}

int release_read_end() {
    int fd = read_end_;

    read_end_ = -1;
    return fd;
}

int release_write_end() {
    int fd = write_end_;

    write_end_ = -1;
    return fd;
}

Pipe(): created_(false), read_end_(-1), write_end_(-1) {
    created_ = create(*this);
}

void close_write_end() {
    
    if (write_end_ < 0) return;
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
