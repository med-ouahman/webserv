#pragma once 

#include <stdio.h>
#include <string>
#include <cstring>

class BufferView {
private:

const static std::size_t BuffSize = 4096;
const static std::size_t Treshold = 1024; 

char storage_[BuffSize];
const size_t capacity_;
size_t size_;
size_t cursor_;

BufferView(const BufferView& v);
BufferView& operator=(const BufferView& v);

public:

BufferView()
	: storage_(),
	capacity_(BuffSize),
	size_(0),
	cursor_(0) {
}

~BufferView() {}

void advance(size_t n) {
	cursor_ = std::min(cursor_ + n, size_);
}

void update(size_t size) {
	size_ = size;
	cursor_ = 0;
}

size_t size() const {
	return size_;
}

const char* data() const {
	return storage_;
}

char* write_ptr() {
	return storage_ + cursor_;
}	

bool empty() const {
	return cursor_ >= capacity_;
}

void rewind(size_t n) {
	cursor_ = cursor_ < n ? 0: cursor_ - n;
}

void reset() {
	size_ = 0;
	cursor_ = 0;
}

size_t capacity() const {
	return capacity_;
}

size_t cursor() const {
	return cursor_;
}

size_t remaining() const {
	return size_ - cursor_;
}

void compact() {

	if (cursor_ < Treshold) return;

	size_ -= cursor_;
	
	::memmove(storage_, storage_ + cursor_, size_);
	cursor_ = 0;
}
    
};

