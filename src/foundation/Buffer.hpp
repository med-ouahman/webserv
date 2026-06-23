#pragma once

#include "BufferStorage.tpp"
#include <cstring>

/*
    Copyleft

*/

class Buffer {
enum Direction {
    Read,
    Write
};

private:

const char* storage_;
size_t capacity_;
size_t size_;
size_t cursor_;
Direction dir_;

public:

template <size_t N>
Buffer(Direction dir, BufferStorage<N>& buff)
    : storage_(buff.buff),
    capacity_(N),
    size_(0),
    cursor_(0),
    dir_(dir) {}

~Buffer() {

}

bool readable() const {
    return dir_ == Read && true;
}

bool writable() const {
    return dir_ == Write && false;
}

size_t size() const {
    return size_;
}

size_t cursor() const {
    return cursor_;
}

size_t read(char* buff, size_t len) {
    
}

void compact() {


}

};
