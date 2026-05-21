#include "BodyReader.hpp"

namespace http {
	
	const std::string BodyReader::hexas = "0123456789abcdef";
	
	BodyReader::BodyReader( int fd, DataView& v )
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
		current_chunk_size(0),
		sc(v) {}

	BodyReader::~BodyReader() {

		if (body_fd >= 0) ::close(body_fd);

		if (body_storage == BodyStorage::FILE_TEMP) {
			::unlink(body_path.c_str());
		}
	}

	ssize_t BodyReader::produce_body_chunk( BufferWriter* writer ) {
		
		if (body_storage == BodyStorage::FILE_TEMP || body_storage == BodyStorage::FILE_PERM) {

			if (body_fd < 0) {
				body_fd = open(body_path.c_str(), O_RDONLY);
				if (body_fd < 0)
					return -1;
			}

			return ::read(body_fd, writer->write_ptr(), writer->remaining());
		}

		size_t to_copy = std::min(body_buff.size(), writer->remaining());
		::memcpy(writer->write_ptr(), body_buff.c_str(), to_copy);
		body_buff.erase(to_copy);
		
		return to_copy;
	}

	void BodyReader::prepare_body( BodyConf& p ) {
		std::cout << "Preparing body\n";
		body_type = p.type;
		body_storage = p.storage;
		body_len = p.parsed_body_size;

		body_state = BodyState::READING;
		
		if (body_storage == BodyStorage::BUFFER)
			return ;
			
		body_path = p.path;
		if (body_storage == BodyStorage::FILE_TEMP) {
			std::stringstream ss;
			ss << conn_fd;
			body_path = "/tmp/tmp_body_" + ss.str(); 
		}
		
		body_fd = open(body_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600);

		if (body_fd < 0) {
			std::cout << "file error\n";
			body_state = BodyState::ERROR;
			return ;
		}

	}



	size_t BodyReader::parse_chunk_size( const std::string& line_buff ) {
		
		size_t chunk_size = 0;
		
		for ( size_t i(0); i < line_buff.size(); ++i ) {

			char c = ::tolower(line_buff[i]);

			if (c == ';') {
				break;
			}

			if (!is_valid_hexa(c)) {
				return MAX_CHUNK_SIZE + 1;
			}

			chunk_size = chunk_size * hexas.size() + hexas.find(c);

			if (chunk_size > MAX_CHUNK_SIZE) {
				/* I will be generous and log errors later */
				return MAX_CHUNK_EXC;
			}
		}

		return chunk_size;
	}


	BodyType::Type Dispatcher::detect_body_type( const Request& request )  {

		if (request.data().method == GET or request.data().method == DELETE) {
			return BodyType::NONE;
		}

		const std::string& transfer_encoding = request.data().headers.get("transfer-encoding");

		bool has_content_length = not request.data().headers.get("content-length").empty();
		
		bool has_transfer_encoding = not transfer_encoding.empty();

		if (has_transfer_encoding and transfer_encoding != "chunked") {
			// build_error_response(NOT_IMPLEMENTED);
			return BodyType::ERROR;	
		}

		if (has_content_length and has_transfer_encoding)
			return BodyType::ERROR;

		if (has_content_length)	return BodyType::CONTENT_LENGTH;
		
		else if (has_transfer_encoding) return BodyType::TRANSFER_ENCODING_CHUNKED;
		
		return BodyType::NONE;
	}


    ::size_t BodyReader::parse_chunk_size( const std::string& line_buff ) {
		
		::size_t chunk_size = 0;
	
		for ( ::size_t i(0); i < line_buff.size(); ++i ) {

			char c = ::tolower(line_buff[i]);

			if (c == ';') {
				break;
			}

			if (!is_valid_hexa(c)) {
				return MAX_CHUNK_SIZE + 1;
			}

			chunk_size = chunk_size * hexas.size() + hexas.find(c);

			if (chunk_size > MAX_CHUNK_SIZE) {
				/* I will be generous and log errors later */
				return MAX_CHUNK_SIZE + 1;
			}
		}
	}


    ScanResult BodyReader::read_body_chunked() {

        if (chunk_state == ChunkState::CHUNK_SIZE) {

            ScanResult r = sc.scan(MAX_HEADER_BLOCK_LEN);

            if (r != SUCCESS) return r;
            
            current_chunk_size = parse_chunk_size(sc.line());

            if (current_chunk_size > MAX_CHUNK_SIZE) return ERROR;
            
            sc.reset();
            
            if (current_chunk_size == 0) chunk_state = ChunkState::CHUNK_LAST;

            else chunk_state = ChunkState::CHUNK_DATA;

        }

        if (chunk_state == ChunkState::CHUNK_LAST) {

            ScanResult res = sc.scan(MAX_HEADER_BLOCK_LEN);
            
            if (res != SUCCESS) return res;

            if (sc.line().size() != 0) return ERROR;

            return SUCCESS;
        }
        
        body_len = current_chunk_size;

        write_body();
        
        if (body_state == BodyState::ERROR)
            return ERROR;
        
        if (body_bytes_parsed == current_chunk_size) {
            body_bytes_parsed = 0;
            sc.reset();
            chunk_state = ChunkState::CHUNK_SIZE;
        }

        return NEED_MORE;
    }


    ScanResult BodyReader::read_body_content_length() {
        
        write_body();
        
        if (body_bytes_parsed == body_len) {
            body_state = BodyState::FINISH;
            std::cout << "Done\n";
        }

        switch (body_state) {
            case BodyState::ERROR:
                return ERROR;
            case BodyState::FINISH:
                return SUCCESS;
            case BodyState::READING:
                return NEED_MORE;
            default:
                return NEED_MORE;
        }

        return SUCCESS;
    }

    void BodyReader::write_body() {

        size_t remaining = body_len - body_bytes_parsed;
        size_t available = data_view.size() - data_view.cursor();
        size_t to_copy = std::min(remaining, available);
        ssize_t copied = 0;

        if (body_storage == BodyStorage::BUFFER) {
            body_buff.append(data_view.data() + data_view.cursor(), to_copy);
            copied = to_copy;
        } else {
            copied = ::write(body_fd, data_view.data() + data_view.cursor(), to_copy); 
           
            if (copied < 0) {
                body_state = BodyState::ERROR;
                return ;
            }
        }

        body_bytes_parsed += copied;
        data_view.advance(copied);
    }

    
    bool BodyReader::is_valid_hexa( const char c ) {
		return hexas.find(c) != std::string::npos;
	}
    
   ScanResult BodyReader::read_body() {

        if (body_type == BodyType::NONE) return SUCCESS;
        
        ScanResult result;
        switch (body_type) {
            case BodyType::CONTENT_LENGTH:
                result = read_body_content_length();
                break;
            case BodyType::TRANSFER_ENCODING_CHUNKED:
                result = read_body_chunked();
                break;
            case BodyType::ERROR:
                result = ERROR;
                break;
            case BodyType::NONE:
                result = SUCCESS;
            default:
                break;
        }

        if (body_state == BodyState::ERROR) return ERROR;
        
        if (result == SUCCESS) reset();

        return result;
    }

    void BodyReader::reset() {
        
        body_bytes_parsed = 0;
        
        body_type = BodyType::NONE;
        body_storage = BodyStorage::NONE;

        if (body_fd >= 0) {
            ::close(body_fd);
            body_fd = -1;
        }
    }
}

