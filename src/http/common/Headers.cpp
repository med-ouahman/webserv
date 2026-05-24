#include "Headers.hpp"

namespace http {

Headers::Headers() {}

Headers::~Headers() {}

Headers::Headers( const Headers& other ): headers_(other.headers_) {}

Headers& Headers::operator=( const Headers& other ) {
    if (this not_eq &other) {
        headers_ = other.headers_;
    }

    return *this;
}

const std::string& Headers::get( const std::string& name ) const {
    static std::string const s("");
    
    for ( size_t i(0); i < headers_.size(); ++i ) {
        if (headers_[i].name == name) return headers_[i].value;
    }

    return s;
}

void Headers::add( std::string const& name, std::string const& value ) {
    if (name.empty()) return ;

    headers_.push_back(Header(name, value));
}

void Headers::remove( std::string const& name ) {

    for ( size_t i(0); i < headers_.size(); ++i ) {
        if (headers_[i].name == name) headers_[i].name = "";
    }

}

bool Headers::has( const std::string& name ) {
    return !get(name).empty();
}

bool Headers::replace( std::string const& name, std::string const& new_value ) {

    if (name.empty()) return false;

    for ( size_t i(0); i < headers_.size(); ++i ) {
        if (headers_[i].name == name) {
            headers_[i].name = new_value;
            return true;
        }
    }

    return false;
}

size_t Headers::size() const {
    return headers_.size();
}

Headers::iterator Headers::begin() {
    return headers_.begin();
}

Headers::iterator Headers::end() {
    return headers_.end();
}

Headers::const_iterator Headers::begin() const {
    return headers_.begin();
}

Headers::const_iterator Headers::end() const {
    return headers_.end();
}

}
