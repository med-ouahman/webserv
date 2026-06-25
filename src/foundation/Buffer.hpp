#pragma once

#include "BufferStorage.tpp"
#include <cstring>
#include <algorithm>

struct Buffer {

char* data_;
const size_t capacity_;
size_t size_;

template <size_t N>
Buffer(BufferStorage<N>& buff)
    : data_(buff.buff),
    capacity_(N),
    size_(0) {}

};
