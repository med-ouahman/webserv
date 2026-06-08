
#include "BufferWriter.hpp"
#include <cstring>
#include <iostream>

BufferWriter::BufferWriter(size_t capacity)
    : used_(0),
    offset_(0) {
    storage_.reserve(capacity);
}

BufferWriter::~BufferWriter() {}

size_t BufferWriter::length() {
    return used_;
}

char* BufferWriter::write_ptr() {
    return &storage_[used_];
}

const char* BufferWriter::base() const {
    return &storage_[0];
}

const char* BufferWriter::read_ptr() const {
    return &storage_[offset_];
}

bool BufferWriter::full() {
    return used_ == storage_.capacity();
}

bool BufferWriter::empty() {
    return used_ == 0;
}

size_t BufferWriter::bytes_pending() {
    return used_ - offset_;
}

size_t BufferWriter::bytes_free() {
    return storage_.capacity() - used_;
}

void BufferWriter::advance_read(size_t n__) {
    offset_ += n__;
}

void BufferWriter::advance_write(size_t n__) {
    used_ += n__;
}

size_t BufferWriter::capacity() {
    return storage_.capacity();
}

void BufferWriter::reset() {
    used_ = 0;
    offset_ = 0;
}

size_t BufferWriter::write(const char* source, size_t n__) {
    size_t available = storage_.capacity() - used_;
    size_t to_copy = std::min(available, n__);
    ::memcpy(write_ptr(), source, to_copy);
    used_ += to_copy;
    return to_copy;
}
