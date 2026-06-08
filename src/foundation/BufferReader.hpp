#pragma once 

#include <stdio.h>
#include <string>

class BufferReader {
private:
    std::string storage_;
    size_t cursor_;

    BufferReader(const BufferReader& v);
    BufferReader& operator=(const BufferReader& v);

public:
    BufferReader(size_t size);
    ~BufferReader();
    void advance(size_t n);
    char* data();
    const char* read_ptr() const;
    bool empty() const;
    void rewind(size_t n);
    void shrink(size_t size);
    void reset();
    size_t cursor() const;
    size_t size() const;
    size_t remaining() const;
    const std::string& str() const;
    
};

