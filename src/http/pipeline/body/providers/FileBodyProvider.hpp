#pragma once

#include "IBodyProvider.hpp"

namespace http {
namespace body {

class FileBodyProvider: public IBodyProvider {
private:
    std::string filename_;
    int fd_;
    size_t size;

public:
    ReadResult read(BufferWriter& w, size_t size);
    FileBodyProvider(const std::string& filename);
    ~FileBodyProvider();
    static size_t file_size(const std::string& filename);
};


}
}
