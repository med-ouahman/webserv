#pragma once 

#include <stdio.h>
#include <string>
#include <cstring>
#include "Storage.tpp"

class BufferView {
private:

const char* storage_;
const size_t size_;

size_t cursor_;

public:

BufferView(const char* storage, size_t size)
: storage_(storage),
size_(size),
cursor_(0) {}

void advance(size_t n) {
	cursor_ += n;
}

const char* data() const {
	return storage_ + cursor_;
}

size_t remaining() const {
	return size_ - cursor_;
};

bool empty() const {
	return cursor_ == size_;
}

size_t size() const {
	return size_;
}

};
