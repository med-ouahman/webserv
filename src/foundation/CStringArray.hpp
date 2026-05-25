#pragma once

#include <vector>
#include <string>
#include <cstring>

class CStringArray {

private:
    std::vector<std::string> storage_;
    mutable std::vector<char*> cache_;
    mutable bool dexpired_;

    void rebuild() const {

        cache_.clear();
        cache_.reserve(storage_.size() + 1);

        for (size_t i = 0; i < storage_.size(); ++i)
            cache_.push_back(const_cast<char*>(storage_[i].c_str()));

        cache_.push_back(NULL);
        dexpired_ = false;
    }

public:
    CStringArray() : dexpired_(false) {}

    void push(const std::string& s) {
        storage_.push_back(s);
        dexpired_ = true;
    }

    void push(const char* s) {
        storage_.push_back(s);
        dexpired_ = true;
    }

    size_t size() const {
        return storage_.size();
    }

    const std::vector<std::string>& data() const {
        return storage_;
    }

    char** argv() const {
        if (dexpired_ || cache_.empty())
            rebuild();

        return cache_.data();
    }
};
