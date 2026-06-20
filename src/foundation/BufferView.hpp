#pragma once 

#include <stdio.h>
#include <string>

class BufferView {
private:

    const static std::size_t BuffSize = 4096;
    
    char storage_[BuffSize];
    const size_t capacity_;
    size_t size_;
    size_t cursor_;

    BufferView(const BufferView& v);
    BufferView& operator=(const BufferView& v);

public:
    BufferView();
    ~BufferView();
    void advance(size_t n);
    void update(size_t n);
    char* write_ptr();
    const char* data() const;
    bool empty() const;
    void rewind(size_t n);
    void reset();

    size_t size() const;
    size_t remaining() const;
    size_t cursor() const;
    size_t capacity() const;
    
};

