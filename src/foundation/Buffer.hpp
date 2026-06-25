#pragma once

#include "BufferStorage.tpp"
#include <cstring>
#include <algorithm>

class Buffer {
enum Direction {
    Read,
    Write
};

char* data_;
const size_t capacity_;
size_t size_;
size_t offset_;

public:
template <size_t N>
Buffer(BufferStorage<N>& buff)
    : data_(buff.buff),
    capacity_(N),
    size_(0),
    offset_(0) {}

const char* read_ptr() const {
    return data_ + offset_;
}

char* write_ptr() {
    return data_ + offset_;
}

size_t offset() const {
    return offset_;
}

size_t size() const {
    return size_;
}

void compact() {

}

size_t capacity() const {
    return capacity_;
}

};
