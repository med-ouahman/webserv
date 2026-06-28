#include "MemoryBodyProvider.hpp"

namespace http {
namespace body {

MemoryBodyProvider::MemoryBodyProvider(std::string const& mem)
  : memory_(mem), offset_(0) {}


MemoryBodyProvider::~MemoryBodyProvider() {
  memory_.clear();
  offset_ = 0;
}


ssize_t MemoryBodyProvider::read(std::string& out, size_t size) {

  if (memory_.empty()) return 0;

  size_t w = std::min(size, memory_.size() - offset_);

  out.append(memory_.c_str() + offset_, w);
  
  offset_ += w;

  if (offset_ == memory_.size()) {
    memory_.clear();
    offset_ = 0;
  }

  return w;

}

}
}
