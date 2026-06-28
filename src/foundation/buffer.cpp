#include "Buffer.hpp"

#include <iostream>

#include <unistd.h>

int main() {


    Storage<2048> b;

    Buffer reader(b);

    std::cout << reader.capacity() << "\n" << reader.size() << "\n";

    ssize_t x = read(0, reader.write_ptr(), reader.capacity());
    
    if (x < 0) return 1;
    reader.advance_write(x);

    std::cout.write(reader.read_ptr(), reader.size()); 

    std::cout << "size: " << reader.size() << "\n";

}
