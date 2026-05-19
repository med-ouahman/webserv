#include "Config.hpp"
#include "Dispatcher.hpp"

namespace http {

    const config::LocationConfig* Dispatcher::find_location( const std::string& url, const std::vector<config::LocationConfig>& locations ) {
    
        config::LocationConfig* location = NULL;
        
        size_t best_prefix_length = 0;

        for ( size_t i(0); i < locations.size(); ++i ) {
            if (url == locations[i].path) {
                return &locations[i];
            }
            
            if (!url.compare(0, locations[i].path.size(), locations[i].path)) {
                if (best_prefix_length < locations[i].path.size()) {
                    best_prefix_length = locations[i].path.size();
                    location = const_cast<config::LocationConfig*>(&locations.at(i));
                }
            }
        }

        return location;
    }
}

