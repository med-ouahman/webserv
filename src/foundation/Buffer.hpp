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
size_t  treshold;

Buffer(const Buffer& other);
Buffer& operator=(const Buffer& other);

public:
template <size_t N>
Buffer(Storage<N>& buf)
: storage_(buf.buff),
capacity_(N),
r_offset_(0),
w_offset_(0),
treshold(0) {}

~Buffer() {}

char* write_ptr() {
    return storage_ + w_offset_;
}

const char* read_ptr() const {
    return storage_ + r_offset_;
}

bool full() const {
    return w_offset_ == capacity_;
}

bool empty() const {
    return w_offset_ == 0;
}

size_t bytes_pending() {
    return w_offset_ - r_offset_;
}

size_t bytes_free() {
    return capacity_ - w_offset_;
}

void advance_read(size_t n__) {
    r_offset_ += n__;
}

void advance_write(size_t n__) {
    w_offset_ += n__;
}

size_t capacity() const {
    return capacity_;
}

void reset() {
    r_offset_ = 0;
    w_offset_ = 0;
}

size_t size() const { 
    return w_offset_;
}

void pop(size_t n) {
    
    if (n > w_offset_) n = w_offset_;
    
    w_offset_ -= n;
}

void compact() {

    size_t len = w_offset_ - r_offset_;

    if (len < capacity_ / 2) return;
    
    ::memmove(storage_, storage_ + r_offset_, len);

    r_offset_ = 0;
    w_offset_ = len;
}

bool writable() const {
    return w_offset_ < capacity_;
}

bool readable() const {
    return w_offset_ > 0;
}

};