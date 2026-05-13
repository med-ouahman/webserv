#include "BufferWriter.hpp"
#include <cstring>

namespace core {


    BufferWriter::BufferWriter( char* b, size_t cap )
        : buff_(b), capacity_(cap), offset_(0), size_(0) {}

    BufferWriter::~BufferWriter() {
        
    }

    void BufferWriter::update( char* buf, size_t n ) {
        buff_ = buf;
        size_ = n;
        offset_ = 0;
    }

    void BufferWriter::update( size_t n ) {
        size_ = n;
        offset_ = 0;
    }
    
    size_t BufferWriter::size() {
        return size_;
    }

    size_t BufferWriter::offset() {
        return offset_;
    }

    char* BufferWriter::data() {
        return buff_ + offset_;
    }

    bool BufferWriter::full() {
        return size_ == capacity_;
    }

    bool BufferWriter::empty() {
        return size_ == 0;
    }

    size_t BufferWriter::remaining() {
        return size_ - offset_;
    }

    void BufferWriter::advance( size_t n__ ) {
        offset_ += n__;
    }

    size_t BufferWriter::capacity() {
        return capacity_;
    }

    void BufferWriter::reset() {
        size_ = 0;
        offset_ = 0;
    }

    char* BufferWriter::write_ptr() {
        return buff_ + offset_;
    }

    size_t BufferWriter::write( const char* source, size_t n__ ) {
        offset_ = 0;
        size_t available = capacity_ - size_;
        size_t to_copy = std::min(available, n__);
        ::memcpy(buff_ + size_, source, available);
        size_ = to_copy;
        return to_copy;
    }

}
