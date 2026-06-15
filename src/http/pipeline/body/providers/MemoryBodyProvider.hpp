#pragma once

#include "IBodyProvider.hpp"

namespace http {
namespace body {

class MemoryBodyProvider: public IBodyProvider {
private:
    std::string memory_;
    size_t      offset_;

public:
    MemoryBodyProvider(const std::string& mem);
    ~MemoryBodyProvider();
    ReadResult read(BufferWriter& w, size_t size);
};

}
}