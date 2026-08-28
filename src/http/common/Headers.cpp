#include "Headers.hpp"

namespace http {

Headers::Headers() {}

Headers::~Headers() {}

Headers::Headers(const Headers& other): headers_(other.headers_) {}

const std::string& Headers::get(const std::string& name) const {
    static std::string const s("");
    
    for (size_t i(0); i < headers_.size(); ++i) {
        if (headers_[i].name == name) return headers_[i].value;
    }

    return s;
}

void Headers::add(std::string const& name, std::string const& value) {
    if (name.empty()) return ;

    headers_.push_back(Header(name, value));
}

Headers::const_iterator Headers::begin() const {
    return headers_.begin();
}

Headers::const_iterator Headers::end() const {
    return headers_.end();
}

}
