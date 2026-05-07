#include "BufferWriter.hpp"

namespace core {


    BufferWriter::BufferWriter( char* b ): buff_(b), capacity_(0), offset_(0) {

    }

    BufferWriter::~BufferWriter() {
        
    }

    void BufferWriter::update( char* buf, size_t n ) {
        buff_ = buf;
        capacity_ = n;
        offset_ = 0;
    }

    void BufferWriter::update( size_t n ) {
        capacity_ = n;
        offset_ = 0;
    }
    
    size_t BufferWriter::size() {
        return capacity_;
    }

    size_t BufferWriter::offset() {
        return offset_;
    }

    char* BufferWriter::data() {
        return buff_ + offset_;
    }

    bool BufferWriter::full() {
        return offset_ == capacity_;
    }

    size_t BufferWriter::remaining() {
        return capacity_ - offset_;
    }

    void BufferWriter::advance( size_t n ) {
        offset_ += n;
    }

    size_t BufferWriter::capacity() {
        return capacity_;
    }

    void BufferWriter::reset() {
        capacity_ = 0;
        offset_ = 0;
    }

    char* BufferWriter::write_ptr() {
        return buff_ + offset_;
    }
}