
#include "Socket.hpp"
#include <cstdlib>

namespace std {
  bool exit(bool a) {
    ::exit(a?0:1);
    return a;
  }
}

std::string generate_random_request() {

  std::string large_header_value = "LARGE_HEADER_VALUE_STRING";

  
  std::string header = "GET / HTTP/1.1\r\nHost: ";

  header.reserve(large_header_value.size()*50);
  for (auto i = 0; i < 50; i++) {
    header += large_header_value;
  }
  
  header.append("\r\n");
  return header;

}


int main(int argc, char** argv) {

  TCPSocket sock(argv[1], std::stoi(argv[2]));

  sock.good()==false ? std::exit(false) : true;

  sock.send_all("GET / HTTP/1.1\r\n");return 0;

  std::string req = generate_random_request();

  while (true) {
    
    sock.send_all(req);

  }

}
