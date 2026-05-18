#pragma once 

#include <stdio.h>


class DataView {
    private:
        const char* data_;
        size_t size_;

        size_t cursor_;

        DataView( const DataView& v );
        DataView& operator=( const DataView& v );

    public:
        DataView( const char* data );
        ~DataView();
        void advance( size_t n );
        const char* data() const;
        const char* read_ptr() const;
        bool empty() const;
        void rewind( size_t n );
        void shrink( size_t size );
        void update( size_t new_size );
        void reset();
        size_t cursor() const;
        size_t size() const;
};

