#include "MemoryBodyProvider.hpp"

namespace http {
namespace body {

MemoryBodyProvider::MemoryBodyProvider(std::string const& mem)
  : memory_(mem), offset_(0) {}


MemoryBodyProvider::~MemoryBodyProvider() {
  memory_.clear();
  offset_ = 0;
}


IBodyProvider::ReadResult MemoryBodyProvider::read(BufferWriter& writer, size_t size) {

  if (offset_ == memory_.size()) return Success;

  size_t to_write = std::min(memory_.size() - offset_, size);
  size_t w = writer.write(memory_.c_str() + offset_, to_write);

  offset_ += w;

  return Success;

}

}
}
