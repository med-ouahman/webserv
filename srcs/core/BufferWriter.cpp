#include "BufferWriter.hpp"

namespace core {


    BufferWriter::BufferWriter( char* b ): buff_(b), size_(0), offset_(0) {

    }

    BufferWriter::~BufferWriter() {
        
    }

    bool BufferWriter::update( char* buf, size_t n ) {
        if (offset_ != size_)
            return false;
        buff_ = buf;
        size_ = n;
        return true;
    }

    size_t BufferWriter::size() {
        return size_;
    }

    size_t BufferWriter::offset() {
        return offset_;
    }

    char* BufferWriter::buff() {
        return buff_ + offset_;
    }

    bool BufferWriter::full() {
        return offset_ == size_;
    }

    size_t BufferWriter::remaining() {
        return size_ - offset_;
    }

    void BufferWriter::advance( size_t n ) {
        offset_ += n;
    }
} 