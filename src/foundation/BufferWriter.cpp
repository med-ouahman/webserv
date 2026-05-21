
#include "BufferWriter.hpp"
#include <cstring>
#include <iostream>

BufferWriter::BufferWriter(): capacity_(BUFFER_SIZE), offset_(0), size_(0) {}

BufferWriter::~BufferWriter() {}

size_t BufferWriter::size() {
    return size_;
}

size_t BufferWriter::offset() {
    return offset_;
}

char* BufferWriter::data() {
    return buff_ + offset_;
}

bool BufferWriter::full() {
    return size_ == capacity_;
}

bool BufferWriter::empty() {
    return size_ == 0;
}

size_t BufferWriter::remaining() {
    return size_ - offset_;
}

size_t BufferWriter::bytes_free() {
    return capacity_ - size_;
}

void BufferWriter::advance( size_t n__ ) {
    offset_ += n__;
}

size_t BufferWriter::capacity() {
    return capacity_;
}

void BufferWriter::reset() {
    size_ = 0;
    offset_ = 0;
}

char* BufferWriter::write_ptr() {
    return buff_ + offset_;
}

size_t BufferWriter::write( const char* source, size_t n__ ) {
    size_t available = capacity_ - size_;
    size_t to_copy = std::min(available, n__);
    ::memcpy(buff_ + size_, source, to_copy);
    size_ += to_copy;
    return to_copy;
}
