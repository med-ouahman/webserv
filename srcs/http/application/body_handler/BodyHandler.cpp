#include "BodyHandler.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <ostream>
#include <iostream>
#include <cstring>

namespace http {
	
	const std::string BodyHandler::hexas = "0123456789abcdef";
	
	BodyHandler::BodyHandler( int fd, core::DataView& v )
		: body_storage(BodyStorage::NONE),
		body_state(BodyState::PREPARING),
		data_view(v),
		conn_fd(fd),
		body_dir("/tmp/"),
		body_bytes_parsed(0),
		body_len(0),
		body_path(""),
		body_fd(-1),
		body_type(BodyType::UNSET),
		body_set(false),
		chunk_state(ChunkState::CHUNK_SIZE),
		chunk_remaining(0),
		sc(v) {}

	BodyHandler::~BodyHandler() {

		if (body_fd >= 0) ::close(body_fd);

		if (body_storage == BodyStorage::FILE_TEMP) {
			::unlink(body_path.c_str());
		}
	}

	ssize_t BodyHandler::produce_body_chunk( char* buff, size_t size ) {
		
		if (body_storage == BodyStorage::FILE_TEMP) {

			if (body_fd < 0) {
				body_fd = open(body_path.c_str(), O_RDONLY);
				if (body_fd < 0)
					return -1;
			}

			return ::read(body_fd, buff, size);
		}

		size_t to_copy = std::min(body_buff.size(), size);
		::memcpy(buff, body_buff.c_str(), to_copy);
		body_buff.erase(to_copy);
		
		return to_copy;
	}

	void BodyHandler::prepare_body( const std::string& filename ) {
		
		if (body_len <= MAX_BODY_BUFF_SIZE) {
			body_storage = BodyStorage::BUFFER;
			return ;
		}
	
		if (filename.size() > 0) {
			body_storage = BodyStorage::FILE_PERM;
			body_path = filename;
		} else {
			std::stringstream ss;
			ss << conn_fd;
			body_path = body_dir + std::string("tmp_body_") + ss.str();
		}
		
		body_fd = open(body_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (body_fd < 0) {
			std::cout << "file error\n";
			body_state = BodyState::ERROR;
			return ;
		}

	}

}