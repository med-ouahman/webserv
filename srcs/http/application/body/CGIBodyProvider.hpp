#include "IBodyProvider.hpp"

namespace http {

    class CGIBodyProvider: public IBodyProvider {

        public:
            bool finished();
            ssize_t read( char* buff, size_t max_size );
            CGIBodyProvider();
            ~CGIBodyProvider();
            
        private:


    };
}