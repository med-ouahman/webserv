#include "BufferReader.hpp"

BufferReader::BufferReader(size_t capacity)
	: storage_(),
	cursor_(0) {
	storage_.resize(capacity, 0);
}

BufferReader::~BufferReader() {}

void BufferReader::advance(size_t n) {
	cursor_ = std::min(cursor_ + n, storage_.size());
}

size_t BufferReader::size() const {
	return storage_.size();
}

char* BufferReader::data() {
	return &storage_[0];
}

const char* BufferReader::read_ptr() const {
	return &storage_[0];
}

size_t BufferReader::cursor() const {
	return cursor_;
}

size_t BufferReader::remaining() const {

	if (cursor_ >= storage_.size()) return 0;

	return storage_.size() - cursor_;
}

bool BufferReader::empty() const {
	return cursor_ >= storage_.size();
}

void BufferReader::rewind(size_t n) {
	cursor_ = cursor_ < n ? 0: cursor_ - n;
}

void BufferReader::reset() {
	cursor_ = 0;
}

const std::string& BufferReader::str() const {
	return storage_;
}

size_t BufferReader::capacity() const {
	return storage_.capacity();
}
