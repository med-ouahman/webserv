#pragma once 

#include <stdio.h>

namespace core {

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
            bool empty() const;
            void rewind( size_t n );
            void shrink( size_t size );
            size_t cursor() const;

    };
}
