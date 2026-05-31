#pragma once

#include <vector>
#include <string>
#include <cstring>

class CStringArray {

private:
    std::vector<std::string> storage_;
    mutable std::vector<char*> cache_;
    mutable bool cache_expired_;

    void rebuild() const {

        cache_.clear();
        cache_.reserve(storage_.size() + 1);

        for (size_t i = 0; i < storage_.size(); ++i)
            cache_.push_back(const_cast<char*>(storage_[i].c_str()));

        cache_.push_back(NULL);
        cache_expired_ = false;
    }

public:
    CStringArray() : cache_expired_(false) {}

    void push(const std::string& s) {
        storage_.push_back(s);
        cache_expired_ = true;
    }

    void push(const char* s) {
        storage_.push_back(s);
        cache_expired_ = true;
    }
    /* __arr should be a NULL terminated array of strings */
    void push_array(const char** __arr) {
        size_t size(0);
        for (; __arr[size]; ++size);
        
        storage_.reserve(size);
        
        for ( size_t i(0); __arr[i]; ++i ) {
            storage_.push_back(__arr[i]);
        }

        cache_expired_ = true;
    }

    size_t size() const {
        return storage_.size();
    }

    const std::vector<std::string>& data() const {
        return storage_;
    }

    char** argv() const {
        if (cache_expired_ || cache_.empty())
            rebuild();

        return cache_.data();
    }
};
