
#include "BufferWriter.hpp"
#include <cstring>
#include <iostream>

BufferWriter::BufferWriter()
    : capacity_(BuffSize),
    used_(0),
    r_offset_(0) {
    
}

BufferWriter::~BufferWriter() {}

size_t BufferWriter::size() {
    return used_;
}

char* BufferWriter::write_ptr() {
    return storage_ + used_;
}

const char* BufferWriter::base() const {
    return storage_;
}

const char* BufferWriter::read_ptr() const {
    return storage_ + r_offset_;
}

bool BufferWriter::full() {
    return used_ == capacity_;
}

bool BufferWriter::empty() {
    return used_ == 0;
}

size_t BufferWriter::bytes_pending() {
    return used_ - r_offset_;
}

size_t BufferWriter::bytes_free() {
    return capacity_ - used_;
}

void BufferWriter::advance_read(size_t n__) {
    r_offset_ += n__;
}

void BufferWriter::advance_write(size_t n__) {
    used_ += n__;
}

size_t BufferWriter::capacity() {
    return capacity_;
}

void BufferWriter::reset() {
    used_ = 0;
    r_offset_ = 0;
}

size_t BufferWriter::write(const char* source, size_t n__) {
    size_t available = capacity_ - used_;
    size_t to_copy = std::min(available, n__);
    ::memcpy(write_ptr(), source, to_copy);
    used_ += to_copy;
    return to_copy;
}


void BufferWriter::pop(size_t n) {
    
    if (n > used_) n = used_;
    
    used_ -= n;
}