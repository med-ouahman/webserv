#pragma once

#include "IBodyProvider.hpp"

namespace http {

namespace body {
class FileBodyProvider: public IBodyProvider {
private:
    std::string filename_;
    int fd_;

public:
    ssize_t read(BufferWriter& w, size_t size);
    FileBodyProvider(const std::string& filename);
    FileBodyProvider(int fd_);
    ~FileBodyProvider();
};


}
}
