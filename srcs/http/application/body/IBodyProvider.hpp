#pragma once

#include <stdio.h>
namespace core {
    class BufferWriter;
}

namespace http {
    /**/
    
    class IBodyProvider {
        public:
            virtual ~IBodyProvider() {}
            virtual ssize_t read( core::BufferWriter* writer, size_t max_size ) = 0;
            virtual bool finished() const = 0;
    };
}
