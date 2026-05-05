#include "IBodyProvider.hpp"
#include "DataView.hpp"

namespace http {

    class CGIHandler;

    class CGIBodyProvider: public IBodyProvider {

        public:
            bool finished() const;
            ssize_t read( char* buff, size_t max_size );
            CGIBodyProvider( CGIHandler& h );
            ~CGIBodyProvider();

        private:
            CGIHandler& cgi_handler;
            core::DataView& data_view;
            CGIBodyProvider( const CGIBodyProvider& other );
            CGIBodyProvider& operator=( const CGIBodyProvider& other );

    };
}