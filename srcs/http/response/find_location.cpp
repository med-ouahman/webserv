#include "Config.hpp"
#include "HTTPResponseHandler.hpp"

namespace http {

    const config::LocationConfig* HTTPResponseHandler::find_location( const std::string& url, const std::vector<config::LocationConfig>& locations ) {
    
        size_t location_index = locations.size();
        size_t best_prefix_length = 0;

        for ( size_t i = 0; i < locations.size(); ++i ) {
            if (url == locations[i].path) {
                return &locations[i];
            }
            if (!url.compare(0, locations[i].path.size(), locations[i].path) && url[locations[i].path.size()] == '/') {
                if (best_prefix_length < locations[i].path.size()) {
                    best_prefix_length = locations[i].path.size();
                    location_index = i;
                }
            }
        }

        if (location_index == locations.size()) {
            return NULL;
        }

        return &locations[location_index];
    }
}

