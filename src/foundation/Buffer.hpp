#pragma once

#include "BufferStorage.tpp"
#include <cstring>
#include <algorithm>

class Buffer {

enum Direction {
    Read,
    Write,
};

private:

char* storage_;

size_t capacity_;
size_t size_;
size_t r_offset_;
size_t w_offset_;
Direction dir_;

public:

template <size_t N>
Buffer(BufferStorage<N>& buff, Direction dir)
    : storage_(buff.buff),
    capacity_(N),
    size_(0),
    r_offset_(0),
    w_offset_(0),
    dir_(dir) {}

~Buffer() {}

char* data() {
    return storage_;
}

void compact() {

}

};
