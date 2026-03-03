#pragma once

namespace http {
    /**/
    
    class IBodyProvider {
        public:
            virtual ~IBodyProvider() {}
            virtual size_t read( char* buff, size_t max_size ) = 0;
            virtual bool finished( void ) const = 0;
    };
}
