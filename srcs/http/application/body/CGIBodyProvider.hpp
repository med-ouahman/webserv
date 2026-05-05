#include "IBodyProvider.hpp"
#include "DataView.hpp"

namespace http {

    class CGIHandler;
    class CGIBodyProvider: public IBodyProvider {

        public:
            bool finished() const;
            ssize_t read( char* buff, size_t max_size );
            CGIBodyProvider( const CGIHandler& h );
            ~CGIBodyProvider();

        private:
            const CGIHandler& cgi_handler;
            core::DataView* view;
    };
}