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
            virtual ssize_t read( core::BufferWriter* writer ) = 0;
            virtual bool finished() const = 0;
    };
}
