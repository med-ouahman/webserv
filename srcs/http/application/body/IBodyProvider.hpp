#pragma once

#include <stdio.h>

class BufferWriter;

namespace http {
    /**/
    
    class IBodyProvider {
        public:
            virtual ~IBodyProvider() {}
            virtual ssize_t read( BufferWriter* writer ) = 0;
            virtual bool finished() const = 0;
    };
}
