#include "DataView.hpp"


DataView::DataView( const char* data_p )
	: data_(data_p),
	size_(0),
	cursor_(0) {}

DataView::~DataView() {}

void DataView::advance( size_t n ) {
	cursor_ += n;
}

size_t DataView::size() const {
	return size_;
}

const char* DataView::data() const {
	return data_;
}

const char* DataView::read_ptr() const {
	return data_ + cursor_;
}

size_t DataView::cursor() const {
	return cursor_;
}

size_t DataView::remaining() const {

	if (cursor_ > size_) return 0;

	return size_ - cursor_;
}

bool DataView::empty() const {
	return cursor_ >= size_;
}

void DataView::rewind( size_t n ) {
	cursor_ -= n;
}

void DataView::shrink( size_t new_size ) {
	size_ = new_size;
}

void DataView::update( size_t new_size ) {
	size_ = new_size;
	cursor_ = 0;
}

void DataView::reset() {
	size_ = 0;
	cursor_ = 0;
}

