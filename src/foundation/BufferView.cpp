#include "BufferView.hpp"

BufferView::BufferView()
	: storage_(),
	capacity_(BuffSize),
	size_(0),
	cursor_(0) {
}

BufferView::~BufferView() {}

void BufferView::advance(size_t n) {
	cursor_ = std::min(cursor_ + n, size_);
}

void BufferView::update(size_t size) {
	size_ = size;
	cursor_ = 0;
}

size_t BufferView::size() const {
	return size_;
}

const char* BufferView::data() const {
	return storage_;
}

char* BufferView::write_ptr() {
	return storage_;
}

bool BufferView::empty() const {
	return cursor_ >= capacity_;
}

void BufferView::rewind(size_t n) {
	cursor_ = cursor_ < n ? 0: cursor_ - n;
}

void BufferView::reset() {
	size_ = 0;
	cursor_ = 0;
}

size_t BufferView::capacity() const {
	return capacity_;
}

size_t BufferView::cursor() const {
	return cursor_;
}

size_t BufferView::remaining() const {
	return size_ - cursor_;
}
