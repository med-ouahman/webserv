#include "BufferReader.hpp"

BufferReader::BufferReader()
	: storage_(),
	capacity_(BuffSize),
	cursor_(0) {
}

BufferReader::~BufferReader() {}

void BufferReader::advance(size_t n) {
	cursor_ = std::min(cursor_ + n, capacity_);
}

size_t BufferReader::size() const {
	return cursor_;
}

char* BufferReader::data() {
	return storage_;
}

const char* BufferReader::read_ptr() const {
	return storage_;
}

size_t BufferReader::remaining() const {

	if (cursor_ >= capacity_) return 0;

	return capacity_ - cursor_;
}

bool BufferReader::empty() const {
	return cursor_ >= capacity_;
}

void BufferReader::rewind(size_t n) {
	cursor_ = cursor_ < n ? 0: cursor_ - n;
}

void BufferReader::reset() {
	cursor_ = 0;
}

size_t BufferReader::capacity() const {
	return capacity_;
}

size_t BufferReader::cursor() const {
	return cursor_;
}
