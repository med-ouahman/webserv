// #include "BodyEncoder.hpp"
// #include <iomanip>
// #include <iostream>
// #include <sstream>

// namespace http {

// BodyEncoder::BodyEncoder(IBodyProvider* b, Encoding e)
//     : body_(b),
//     encoding_(e) {}

// BodyEncoder::~BodyEncoder() {

// }

// ssize_t BodyEncoder::read(BufferWriter& w) {
    
//     switch (encoding_) {
//         case CONTENT_LENGTH:
//             lengthed_.write(w);
//             return n;
//         case CHUNKED:
//             return chunked_.write(w);
//         default:
//             break;
//     }

//     return 0;
// }

// std::string& ChunkedEncoder::format_chunk(size_t chunk_size) {
//     std::stringstream ss;
//     ss << std::hex << chunk_size;
//     formatted = ss.str();
//     return formatted;
// }

// ssize_t ChunkedEncoder::write(BufferWriter& w) {
//     switch (state_) {
//         case HEAD: {
//             ca
//             std::string s('\0', max_chunk_header);
//             w.write(s.c_str(), max_chunk_header);
//         }

//         case DATA:

//         case TRAIL:

//         case LAST:

//         default:
//             break;
//     }

//     return 0;
// }

// }
