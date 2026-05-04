#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include "DataView.hpp"

namespace http {

    CGIBodyProvider::CGIBodyProvider( const CGIHandler& h, core::DataView* v )
        : cgi_handler(h),
        view(v)
    {}

    CGIBodyProvider::~CGIBodyProvider() {

    }

    bool CGIBodyProvider::finished() const {

    }

    ssize_t CGIBodyProvider::read( char* buff, size_t size ) {
 
        cgi_handler.pull();
        size_t to_copy = std::min(view->len_ - view->bytes_consumed, size);
        ::memcpy(buff, view->data_ptr_, to_copy);
        return to_copy;
    }

}
