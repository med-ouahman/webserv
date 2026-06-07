#pragma once
#include <stdio.h>
namespace http {
    /**/
    
    class IBodyProvider {
        public:
            virtual ~IBodyProvider() {}
            virtual ssize_t read( char* buff, ::size_t max_size ) = 0;
            virtual bool finished( void ) const = 0;
    };
}
