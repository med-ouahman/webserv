#include "CGIBodyProvider.hpp"
#include "CGIHandler.hpp"

namespace http {

    CGIBodyProvider::CGIBodyProvider( CGIHandler& h )
        : cgi_handler(h),
        data_view(h.get_stdout_data_view())
    {}

    CGIBodyProvider::~CGIBodyProvider() {

    }

    bool CGIBodyProvider::finished() const {
        return true;
    }

    ssize_t CGIBodyProvider::read( char* buff, size_t size ) {
 
        cgi_handler.pull();
        size_t to_copy = std::min(data_view.size() - data_view.cursor(), size);
        ::memcpy(buff, data_view.data(), to_copy);
        return to_copy;
    }

}
