#pragma once

#include <stddef.h>
#include <string>
#include <cstring>

#include "BufferStorage.tpp"

class BufferWriter {

private:

char*   storage_;
size_t  capacity_;
size_t  used_;
size_t	r_offset_;
size_t  treshold;

BufferWriter(const BufferWriter& other);
BufferWriter& operator=(const BufferWriter& other);

public:
template <size_t N>
BufferWriter(BufferStorage<N>& buf)
    : storage_(buf.buff),
    capacity_(N),
    used_(0),
    r_offset_(0),
    treshold(0) {}

BufferWriter()
    : storage_(NULL),
    capacity_(0),
    used_(0),
    r_offset_(0),
    treshold(0) {}

~BufferWriter() {}

size_t size() const {
    return used_;
}

char* write_ptr() {
    return storage_ + used_;
}

const char* read_ptr() const {
    return storage_ + r_offset_;
}

bool full() {
    return used_ == capacity_;
}

bool empty() {
    return used_ == 0;
}

size_t bytes_pending() {
    return used_ - r_offset_;
}

size_t bytes_free() {
    return capacity_ - used_;
}

void advance_read(size_t n__) {
    r_offset_ += n__;
}

void advance_write(size_t n__) {
    used_ += n__;
}

size_t capacity() {
    return capacity_;
}

void reset() {
    used_ = 0;
    r_offset_ = 0;
}

size_t written() const {
    return r_offset_;
}

size_t write(const char* source, size_t n__) {
    size_t available = capacity_ - used_;

    size_t to_copy = std::min(available, n__);
    
    ::memcpy(write_ptr(), source, to_copy);
    
    used_ += to_copy;

    return to_copy;
}


void pop(size_t n) {
    
    if (n > used_) n = used_;
    
    used_ -= n;
}

void compact() {

    if (r_offset_ < treshold) return;

    used_ -= r_offset_;

    ::memmove(storage_, storage_ + r_offset_, used_);
    r_offset_ = 0;
}

};