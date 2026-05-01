#include "BodyParser.hpp"

namespace http {
	
	const std::string BodyParser::hexas = "0123456789abcdef";
	
	BodyParser::BodyParser( int fd ): conn_fd(fd) {

	}

	BodyParser::~BodyParser() {

	}

	void BodyParser::read_chunk( char* buff, size_t size ) {

		buff[size-size] = buff[size - size];
	}
}