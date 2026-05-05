#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"
#include "DataView.hpp"

namespace http {

    CGIBodyProvider::CGIBodyProvider( const CGIHandler& h )
        : cgi_handler(h)
    {}

    CGIBodyProvider::~CGIBodyProvider() {

    }

    bool CGIBodyProvider::finished() const {
        return true;
    }

    ssize_t CGIBodyProvider::read( char* buff, size_t size ) {
 
        // cgi_handler.pull();
        size_t to_copy = std::min(view->size() - view->cursor(), size);
        ::memcpy(buff, view->data(), to_copy);
        return to_copy;
    }

}
