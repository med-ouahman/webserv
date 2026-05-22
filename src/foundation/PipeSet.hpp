#pragma once

#include "UniqueFd.hpp"
#include <unistd.h>


struct PipeSet {
    
    private:
        bool created_;
        UniqueFd read_end_;
        UniqueFd write_end_;

    static bool create(PipeSet& out) {
        int fds[2];

        if (::pipe(fds) == -1)
            return false;

        out.read_end_.set(fds[0]);
        out.write_end_.set(fds[1]);
        return true;
    }

    void close() {
        read_end_.reset();
        write_end_.reset();
    }

    const UniqueFd& read_end() const {
        return read_end_;
    }

    const UniqueFd& write_end() const {
        return write_end_;
    }

    PipeSet(): created_(create(*this)) {}

    ~PipeSet() {
        read_end_.reset();
        write_end_.reset();
    }
};

