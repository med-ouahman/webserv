#pragma once

#include <stddef.h>
#include <string>
#include <cstring>
#include "Storage.tpp"

class Buffer {

private:

char*   storage_;
size_t  capacity_;
size_t	r_offset_;
size_t  w_offset_;


Buffer(const Buffer& other);
Buffer& operator=(const Buffer& other);

public:
template <size_t N>
Buffer(Storage<N>& buf)
    : storage_(buf.buff),
    capacity_(N),
    r_offset_(0),
    w_offset_(0) {}

~Buffer() {}

char* write_ptr() {
    return storage_ + w_offset_;
}

const char* read_ptr() const {
    return storage_ + r_offset_;
}

bool empty() const {
    return bytes_pending() == 0;
}

size_t bytes_pending() const {
    return w_offset_ - r_offset_;
}

size_t bytes_free() const {
    return capacity_ - w_offset_;
}

void advance_read(size_t n__) {
    r_offset_ += n__;
}

void advance_write(size_t n__) {
    w_offset_ += n__;
}

size_t size() const { 
    return w_offset_;
}

void compact() {

    size_t len = w_offset_ - r_offset_;

    ::memmove(storage_, storage_ + r_offset_, len);

    r_offset_ = 0;
    w_offset_ = len;
}

};
