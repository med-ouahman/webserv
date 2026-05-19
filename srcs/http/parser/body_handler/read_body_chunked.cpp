#include "BodyHandler.hpp"
#include <unistd.h>

namespace http {

    ScanResult BodyHandler::read_body_chunked() {

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
}

