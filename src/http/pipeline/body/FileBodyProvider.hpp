#pragma once

#include "IBodyProvider.hpp"

namespace http {
namespace body {

class FileBodyProvider: public IBodyProvider {
private:
    int fd_;
    std::string filename_;
    size_t size;

public:
    ssize_t read(BufferWriter& wr);
    FileBodyProvider(const std::string& filename);
    ~FileBodyProvider();
    static size_t file_size(const std::string& filename);
};


}
}
