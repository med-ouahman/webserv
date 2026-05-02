#include "BodyParser.hpp"
#include <unistd.h>

namespace http {

    ScanResult BodyParser::parse_body_chunked() {

        if (chunk_state == ChunkState::CHUNK_SIZE) {

            ScanResult r = sc.scan(MAX_HEADER_BLOCK_LEN);

            if (r != SUCCESS) return r;
            
            chunk_remaining = parse_chunk_size(sc.line());

            if (chunk_remaining > MAX_CHUNK_SIZE) return ERROR;
            
            sc.reset();
            
            if (chunk_remaining == 0) chunk_state = ChunkState::CHUNK_LAST;

            else chunk_state = ChunkState::CHUNK_DATA;
    
        }

        if (chunk_state == ChunkState::CHUNK_LAST) {

            ScanResult res = sc.scan(MAX_HEADER_BLOCK_LEN);
            
            if (res != SUCCESS) return res;

            if (sc.line().size() != 0) return ERROR;

            return SUCCESS;
        }
        
        body_len = chunk_remaining;

        write_body();
        
        if (body_bytes_parsed == chunk_remaining) {
            body_bytes_parsed = 0;
            sc.reset();
            chunk_state = ChunkState::CHUNK_SIZE;
        }

        return NEED_MORE;
    }
}

